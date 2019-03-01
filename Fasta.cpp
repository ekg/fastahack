// ***************************************************************************
// FastaIndex.cpp (c) 2010 Erik Garrison <erik.garrison@bc.edu>
// Marth Lab, Department of Biology, Boston College
// All rights reserved.
// ---------------------------------------------------------------------------
// Last modified: 26 February 2019 (AMN)
// ---------------------------------------------------------------------------

#include "Fasta.h"

FastaIndexEntry::FastaIndexEntry(string name, int length, long long offset, int line_blen, int line_len)
    : name(name)
    , length(length)
    , offset(offset)
    , line_blen(line_blen)
    , line_len(line_len)
{}

FastaIndexEntry::FastaIndexEntry(void) // empty constructor
{ clear(); }

FastaIndexEntry::~FastaIndexEntry(void)
{}

void FastaIndexEntry::clear(void)
{
    name = "";
    length = 0;
    offset = -1;  // no real offset will ever be below 0, so this allows us to
                  // check if we have already recorded a real offset
    line_blen = 0;
    line_len = 0;
}

ostream& operator<<(ostream& output, const FastaIndexEntry& e) {
    // just write the first component of the name, for compliance with other tools
    output << split(e.name, ' ').at(0) << "\t" << e.length << "\t" << e.offset << "\t" <<
        e.line_blen << "\t" << e.line_len;
    return output;  // for multiple << operators.
}

FastaIndex::FastaIndex(void) 
{}

void FastaIndex::readIndexFile(string fname) {
    string line;
    long long linenum = 0;
    indexFile.open(fname.c_str(), ifstream::in);
    if (indexFile.is_open()) {
        while (getline (indexFile, line)) {
            ++linenum;
            // the fai format defined in samtools is tab-delimited, every line being:
            // fai->name[i], (int)x.len, (long long)x.offset, (int)x.line_blen, (int)x.line_len
            vector<string> fields = split(line, '\t');
            if (fields.size() == 5) {  // if we don't get enough fields then there is a problem with the file
                // note that fields[0] is the sequence name
                char* end;
                string name = split(fields[0], " \t").at(0);  // key by first token of name
                sequenceNames.push_back(name);
                this->insert(make_pair(name, FastaIndexEntry(fields[0], atoi(fields[1].c_str()),
                                                    strtoll(fields[2].c_str(), &end, 10),
                                                    atoi(fields[3].c_str()),
                                                    atoi(fields[4].c_str()))));
            } else {
                cerr << "Warning: malformed fasta index file " << fname << 
                    "does not have enough fields @ line " << linenum << endl;
                cerr << line << endl;
                exit(1);
            }
        }
    } else {
        cerr << "could not open index file " << fname << endl;
        exit(1);
    }
}

// for consistency this should be a class method
bool fastaIndexEntryCompare ( FastaIndexEntry a, FastaIndexEntry b) { return (a.offset<b.offset); }

ostream& operator<<(ostream& output, FastaIndex& fastaIndex) {
    vector<FastaIndexEntry> sortedIndex;
    for(vector<string>::const_iterator it = fastaIndex.sequenceNames.begin(); it != fastaIndex.sequenceNames.end(); ++it)
    {
        sortedIndex.push_back(fastaIndex[*it]);
    }
    sort(sortedIndex.begin(), sortedIndex.end(), fastaIndexEntryCompare);
    for( vector<FastaIndexEntry>::iterator fit = sortedIndex.begin(); fit != sortedIndex.end(); ++fit) {
        output << *fit << endl;
    }
    return output;
}

// Read a line from in into line. Line endings ('\r', '\n', etc.) are not
// written to line but is counted in bytes, which will hold the total bytes
// consumed. Supports both '\n' and '\r\n' line endings. Returns true if data
// was read, and false on EOF before anything could be read.
bool getlineCounting(istream& in, string& line, int& bytes) {
    bytes = 0;
    line.clear();
    for(int got = in.get(); got != EOF; got = in.get()) {
        bytes++;
        if (got == '\n') {
            // Line is over, but we read something (the '\n')
            return true;
        } else if (got != '\r') {
            // Anything other than a '\r' is real data.
            line.push_back((char)got);
        }
        // '\r' is skipped, but still counted in bytes
    }
    return !line.empty();
}

void FastaIndex::indexReference(string refname) {
    // overview:
    //  for line in the reference fasta file
    //  track byte offset from the start of the file
    //  if line is a fasta header, take the name and dump the last sequnece to the index
    //  if line is a sequence, add it to the current sequence
    //cerr << "indexing fasta reference " << refname << endl;
    string line;
    FastaIndexEntry entry;  // an entry buffer used in processing
    entry.clear();
    int line_length = 0;
    int line_bytes = 0;
    long long offset = 0;  // byte offset from start of file
    long long line_number = 0; // current line number
    bool mismatchedLineLengths = false; // flag to indicate if our line length changes mid-file
                                        // this will be used to raise an error
                                        // if we have a line length change at
                                        // any line other than the last line in
                                        // the sequence
    bool emptyLine = false;  // flag to catch empty lines, which we allow for
                             // index generation only on the last line of the sequence
    ifstream refFile;
    refFile.open(refname.c_str());
    if (refFile.is_open()) {
        while (getlineCounting(refFile, line, line_bytes)) {
            ++line_number;
            line_length = line.length();
            if (line[0] == ';') {
                // fasta comment, skip
            } else if (line[0] == '+') {
                // fastq quality header
                
                // account for header offset
                offset += line_bytes;
                
                // read in quality line so its offset will be accounted for too
                // TODO: we don't support the quality offset field of the FAI format
                getlineCounting(refFile, line, line_bytes);
                line_length = line.length();
            } else if (line[0] == '>' || line[0] == '@') { // fasta /fastq header
                // if we aren't on the first entry, push the last sequence into the index
                if (entry.name != "") {
                    mismatchedLineLengths = false; // reset line length error tracker for every new sequence
                    emptyLine = false;
                    flushEntryToIndex(entry);
                    entry.clear();
                }
                entry.name = line.substr(1, line_length - 1);
            } else { // we assume we have found a sequence line
                if (entry.offset == -1) // NB initially the offset is -1
                    entry.offset = offset;
                entry.length += line_length;
                if (entry.line_len) {
                    if (mismatchedLineLengths || emptyLine) {
                        if (line_length == 0) {
                            emptyLine = true; // flag empty lines, raise error only if this is embedded in the sequence
                        } else {
                            if (emptyLine) {
                                cerr << "ERROR: embedded newline";
                            } else {
                                cerr << "ERROR: mismatched line lengths";
                            }
                            cerr << " at line " << line_number << " within sequence " << entry.name <<
                                endl << "File not suitable for fasta index generation." << endl;
                            exit(1);
                        }
                    }
                    // this flag is set here and checked on the next line
                    // because we may have reached the end of the sequence, in
                    // which case a mismatched line length is OK
                    if (entry.line_len != line_bytes) {
                        mismatchedLineLengths = true;
                        if (line_length == 0) {
                            emptyLine = true; // flag empty lines, raise error only if this is embedded in the sequence
                        }
                    }
                } else {
                    entry.line_len = line_bytes; // first line
                    entry.line_blen = line_length;
                }
            }
            offset += line_bytes;
        }
        // we've hit the end of the fasta file!
        // flush the last entry
        flushEntryToIndex(entry);
    } else {
        cerr << "could not open reference file " << refname << " for indexing!" << endl;
        exit(1);
    }
}

void FastaIndex::flushEntryToIndex(FastaIndexEntry& entry) {
    string name = split(entry.name, " \t").at(0);  // key by first token of name
    sequenceNames.push_back(name);
    this->insert(make_pair(name, FastaIndexEntry(entry.name, entry.length,
                        entry.offset, entry.line_blen,
                        entry.line_len)));

}

void FastaIndex::writeIndexFile(string fname) {
    //cerr << "writing fasta index file " << fname << endl;
    ofstream file;
    file.open(fname.c_str()); 
    if (file.is_open()) {
        file << *this;
    } else { 
        cerr << "could not open index file " << fname << " for writing!" << endl;
        exit(1);
    }
}

FastaIndex::~FastaIndex(void) {
    indexFile.close();
}

FastaIndexEntry FastaIndex::entry(string name) {
    FastaIndex::iterator e = this->find(name);
    if (e == this->end()) {
        cerr << "unable to find FASTA index entry for '" << name << "'" << endl;
        exit(1);
    } else {
        return e->second;
    }
}

string FastaIndex::indexFileExtension() { return ".fai"; }

/*
FastaReference::FastaReference(string reffilename) {
}
*/

void FastaReference::open(string reffilename) {
    filename = reffilename;
    if (!(file = fopen(filename.c_str(), "r"))) {
        cerr << "could not open " << filename << endl;
        exit(1);
    }
    index = new FastaIndex();
    struct stat stFileInfo; 
    string indexFileName = filename + index->indexFileExtension(); 
    // if we can find an index file, use it
    if(stat(indexFileName.c_str(), &stFileInfo) == 0) { 
        index->readIndexFile(indexFileName);
    } else { // otherwise, read the reference and generate the index file in the cwd
        cerr << "index file " << indexFileName << " not found, generating..." << endl;
        index->indexReference(filename);
        index->writeIndexFile(indexFileName);
    }
}

FastaReference::~FastaReference(void) {
    if (file != NULL)
      fclose(file);
    if (index != NULL)
      delete index;
}

string FastaReference::getSequence(string seqname) {
    FastaIndexEntry entry = index->entry(seqname);
    int bytes_per_newline = entry.line_len - entry.line_blen;
    int newline_bytes_in_sequence = entry.length / entry.line_blen * bytes_per_newline;
    int seqlen = newline_bytes_in_sequence + entry.length;
    char* seq = (char*) calloc (seqlen + 1, sizeof(char));
    fseek64(file, entry.offset, SEEK_SET);
    string s;
    if (fread(seq, sizeof(char), seqlen, file)) {
        seq[seqlen] = '\0';
        char* pbegin = seq;
        char* pend = seq + (seqlen/sizeof(char));
        pend = remove(pbegin, pend, '\r');
        pend = remove(pbegin, pend, '\n');
        pend = remove(pbegin, pend, '\0');
        s = seq;
        free(seq);
        s.resize((pend - pbegin)/sizeof(char));
    }
    return s;
}

// TODO cleanup; odd function.  use a map
string FastaReference::sequenceNameStartingWith(string seqnameStart) {
    try {
        return (*index)[seqnameStart].name;
    } catch (exception& e) {
        cerr << e.what() << ": unable to find index entry for " << seqnameStart << endl;
        exit(1);
    }
}

string FastaReference::getTargetSubSequence(FastaRegion& target) {
    if (target.startPos == -1) {
        return getSequence(target.startSeq);
    } else {
        return getSubSequence(target.startSeq, target.startPos - 1, target.length());
    }
}

string FastaReference::getSubSequence(string seqname, int start, int length) {
    FastaIndexEntry entry = index->entry(seqname);
    length = min(length, entry.length - start);
    if (start < 0 || length < 1) {
        //cerr << "Empty sequence" << endl;
        return "";
    }
    // we have to handle newlines
    // approach: count newlines before start
    //           count newlines by end of read
    //             subtracting newlines before start find count of embedded newlines
    int newlines_before = start > 0 ? (start - 1) / entry.line_blen : 0;
    int newlines_by_end = (start + length - 1) / entry.line_blen;
    int newlines_inside = newlines_by_end - newlines_before;
    int bytes_per_newline = entry.line_len - entry.line_blen;
    int seqlen = length + newlines_inside * bytes_per_newline;
    char* seq = (char*) calloc (seqlen + 1, sizeof(char));
    fseek64(file, (off_t) (entry.offset + newlines_before + start), SEEK_SET);
    string s;
    if (fread(seq, sizeof(char), (off_t) seqlen, file)) {
        seq[seqlen] = '\0';
        char* pbegin = seq;
        char* pend = seq + (seqlen/sizeof(char));
        pend = remove(pbegin, pend, '\r');
        pend = remove(pbegin, pend, '\n');
        pend = remove(pbegin, pend, '\0');
        s = seq;
        free(seq);
        s.resize((pend - pbegin)/sizeof(char));
    }
    return s;
}

long unsigned int FastaReference::sequenceLength(string seqname) {
    FastaIndexEntry entry = index->entry(seqname);
    return entry.length;
}

