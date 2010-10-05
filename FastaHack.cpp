#include "Fasta.h"
#include <stdlib.h>
#include <getopt.h>
#include "disorder.h"

void printSummary() {
    cerr << "usage: fastahack [options] <fasta reference>" << endl
         << endl
         << "options:" << endl 
         << "    -i, --index          generate fasta index <fasta reference>.fai" << endl
         << "    -r, --region REGION  print the specified region" << endl
         //<< "    -l, --length         print the length of the specified region or sequence" << endl
         << "    -e, --entropy        print the shannon entropy of the specified region" << endl
         << endl
         << "REGION is of the form <seq>, <seq>:<start>..<end>, <seq1>:<start>..<seq2>:<end>" << endl
         << "where start and end are 1-based.  Specifying a sequence name alone will return" << endl
         << "the entire sequence, while specifying a range will return that range." << endl
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

    bool buildIndex = false;  // flag to force index building
    bool printEntropy = false;  // entropy printing
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
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hier:",
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
 
          case 'i':
            buildIndex = true;
            break;
 
          case 'r':
            region = optarg;
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

    FastaReference* fr = new FastaReference(fastaFileName);
    if (region != "") {

        string startSeq;
        int startPos;
        int stopPos;

        size_t foundFirstColon = region.find(":");

        // we only have a single string, use the whole sequence as the target
        if (foundFirstColon == string::npos) {
            startSeq = region;
            //startPos = 1;
            //stopPos = -1;
            sequence = fr->getSequence(region);
        } else {
            startSeq = region.substr(0, foundFirstColon);
            size_t foundRangeDots = region.find("..", foundFirstColon);
            if (foundRangeDots == string::npos) {
                startPos = atoi(region.substr(foundFirstColon + 1).c_str());
                stopPos = startPos + 1; // just print one base if we don't give an end
            } else {
                startPos = atoi(region.substr(foundFirstColon + 1, foundRangeDots - foundRangeDots - 1).c_str());
                stopPos = atoi(region.substr(foundRangeDots + 2).c_str()); // to the start of this chromosome
            }
            int length = stopPos - startPos;
            sequence = fr->getSubSequence(startSeq, startPos - 1, length);
        }

    }

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

    return 0;
}
