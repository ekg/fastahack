#include "Fasta.h"
#include <stdlib.h>
#include <getopt.h>
#include "disorder.h"
#include "Region.h"

void printSummary() {
    cerr << "usage: fastahack [options] <fasta reference>" << endl
         << endl
         << "options:" << endl 
         << "    -i, --index          generate fasta index <fasta reference>.fai" << endl
         << "    -r, --region REGION  print the specified region" << endl
         << "    -c, --stdin          read a stream of line-delimited region specifiers on stdin" << endl
         << "                         and print the corresponding sequence for each on stdout" << endl
         << "    -e, --entropy        print the shannon entropy of the specified region" << endl
         << "    -d, --dump           print the fasta file in the form 'seq_name <tab> sequence'" << endl
         << endl
         << "REGION is of the form <seq>, <seq>:<start>[sep]<end>, <seq1>:<start>[sep]<seq2>:<end>" << endl
         << "where start and end are 1-based, and the region includes the end position." << endl
         << "[sep] is \"-\" or \"..\"" << endl
         << endl
         << "Specifying a sequence name alone will return the entire sequence, specifying" << endl
         << "range will return that range, and specifying a single coordinate pair, e.g." << endl
         << "<seq>:<start> will return just that base." << endl
         << endl
         << "author: Erik Garrison <erik.garrison@bc.edu>" << endl;
}


int main (int argc, char** argv) {

    string command;
    string fastaFileName;
    string seqname;
    string longseqname;
    long long start;
    long long length;
    bool dump = false;

    bool buildIndex = false;  // flag to force index building
    bool printEntropy = false;  // entropy printing
    bool readRegionsFromStdin = false;
    //bool printLength = false;
    string region;

    int c;

    while (true) {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            //{"verbose", no_argument,       &verbose_flag, 1},
            //{"brief",   no_argument,       &verbose_flag, 0},
            {"help", no_argument, 0, 'h'},
            {"index",  no_argument, 0, 'i'},
            //{"length",  no_argument, &printLength, true},
            {"entropy", no_argument, 0, 'e'},
            {"region", required_argument, 0, 'r'},
            {"stdin", no_argument, 0, 'c'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hciedr:",
                         long_options, &option_index);

      /* Detect the end of the options. */
          if (c == -1)
            break;
 
          switch (c)
            {
            case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
              break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
              printf (" with arg %s", optarg);
            printf ("\n");
            break;

          case 'e':
            printEntropy = true;
            break;

          case 'c':
            readRegionsFromStdin = true;
            break;
 
          case 'i':
            buildIndex = true;
            break;
 
          case 'r':
            region = optarg;
            break;

            case 'd':
                dump = true;
                break;

          case 'h':
            printSummary();
            exit(0);
            break;
 
          case '?':
            /* getopt_long already printed an error message. */
            printSummary();
            exit(1);
            break;
 
          default:
            abort ();
          }
      }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        //cerr << "fasta file: " << argv[optind] << endl;
        fastaFileName = argv[optind];
    } else {
        cerr << "please specify a fasta file" << endl;
        printSummary();
        exit(1);
    }

    if (buildIndex) {
        FastaIndex* fai = new FastaIndex();
        //cerr << "generating fasta index file for " << fastaFileName << endl;
        fai->indexReference(fastaFileName);
        fai->writeIndexFile((string) fastaFileName + fai->indexFileExtension());
    }
    
    string sequence;  // holds sequence so we can optionally process it

    FastaReference fr;
    fr.open(fastaFileName);

    if (dump) {
        for (vector<string>::iterator s = fr.index->sequenceNames.begin(); s != fr.index->sequenceNames.end(); ++s) {
            cout << *s << "\t" << fr.getSequence(*s) << endl;
        }
        return 0;
    }

    if (region != "") {
        FastaRegion target(region);
        sequence = fr.getTargetSubSequence(target);
    }

    if (readRegionsFromStdin) {
        string regionstr;
        while (getline(cin, regionstr)) {
            FastaRegion target(regionstr);
            if (target.startPos == -1) {
                cout << fr.getSequence(target.startSeq) << endl;
            } else {
                cout << fr.getSubSequence(target.startSeq, target.startPos - 1, target.length()) << endl;
            }
        }
    } else {
        if (sequence != "") {
            if (printEntropy) {
                if (sequence.size() > 0) {
                    cout << shannon_H((char*) sequence.c_str(), sequence.size()) << endl;
                } else {
                    cerr << "please specify a region or sequence for which to calculate the shannon entropy" << endl;
                }
            } else {  // if no statistical processing is requested, just print the sequence
                cout << sequence << endl;
            }
        }
    }

    return 0;
}
