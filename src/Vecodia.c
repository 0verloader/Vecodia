/********************************************************************************
* Author: Konstantinos Arakadakis                                               *
* Last Modified: 18/5/2022                                                      *
*********************************************************************************
*                                                                               *
*                       VEry COmpact DIffencing Algorithm                       *
*                                   (Vecodia)                                   *
*                                                                               *
* Computes the optimal delta scipt, given the legacy and the updated file.      *
* The legacy file represents the file version that a receiptient party already  *
* has, whereas the updated one is a newer file version.                         *
* To do so, Vecodia detects all common segments between the two file versions,  *
* in both forward and reverse order, as well as the common segments found       *
* in the partially reconstructed update file, during the reconstruction stage.  *
* Once the delta script has been received by a receiptient, the instructions    *
* that it contains need to be interpretted, resulting in a segment of the       *
* updated file, progressively reconstrution the update file version, locally.   *
********************************************************************************/


#include "include/misc.h"
#include "include/suffix_array.h"
#include "include/commands_computation.h"
/*
#include "include/suffix_array.h"
#include "include/commands_computation.h"
#include "include/reconstruct.h"
#include "include/divsufsort.h"
*/

#include <stdint.h>
#include <linux/limits.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define LEGACY 1
#define SUDO_MAX_PATH 4096
#define VERBOSE 1
#define DEFAULT_DELTA_BIN "./delta.bin"
/*******************************************************************************/
int main(int argc, char **argv){
  struct proc_commands f_commands;
  uint path_max;
  int reconstruction = 0;
  int padding = 0;
  int executables = 1;
  int in_place = 0;
  char* legacy_f = NULL;
  char* update_f = NULL;
  char* delta_f = strdup(DEFAULT_DELTA_BIN);
  FILE *fp1, *fp2, *fp3;
  uint8_t stop = 0;
  opterr = 0;

  #ifdef PATH_MAX
    path_max = PATH_MAX;
  #else
    path_max = pathconf(path, _PC_PATH_MAX);
    if (path_max <= 0)
      path_max = SUDO_MAX_PATH;
  #endif

  char abs_legacy_f[path_max];
  char abs_update_f[path_max];
  char abs_delta_f[path_max];

  #if !LEGACY
  /* 
  long_options functionality from 
  http://boron.physics.metu.edu.tr/ozdogan/SystemsProgramming/ceng425/node22.html
  */
  int next_option;
  const char* const short_options = "l:u:d:";
  extern int opterr;

  const struct option long_options[] = {
    { "help",                           0, NULL, 'h' },
    { "reconstruction",                 0, NULL, 'r' },
    { "executables-oriented",           0, NULL, 'e' },
    { "generic",                        0, NULL, 'g' },
    { "in-place",                       0, NULL, 'i' },
    { "out-of-place",                   0, NULL, 'o' },
    { "padding",                        0, NULL, 'p' },
    { "legacy-file",                    1, NULL, 'l' },
    { "update-file",                    1, NULL, 'u' },
    { "delta-file",                     1, NULL, 'd' },
    { NULL,                             0, NULL,  0  } 
  };

  do {
    next_option = getopt_long (argc, argv, short_options,
                               long_options, NULL);

    switch (next_option)
    {
      case '?':
      case 'h':
      default:
        printf("--help\n");
        exit(EXIT_FAILURE);

      case 'r':
        reconstruction = 1;
        break;

      case 'i':
        in_place = 1;
        break;

      case 'p':
        padding = 1;
        break;

      case 'o':
        in_place = 0;
        break;

      case 'e':
        executables = 1;
        break;

      case 'g':
        executables = 0;
        break;

      case 'l':
        legacy_f = strdup(optarg);
        break;

      case 'u':
        update_f = strdup(optarg);
        break;

      case 'd':
        delta_f = strdup(optarg);
        break;

      case -1: /* Done with options. */
        break;    }
  } while (next_option != -1);

  #else

  if (argc < 3){
    printf("--help\n");
    exit(EXIT_FAILURE);
  }

  legacy_f = strdup(argv[1]);
  update_f = strdup(argv[2]);

  if(argc > 3)
      delta_f = strdup(argv[3]);

  executables = 1;
  in_place = 1;
  reconstruction = 0;
  #endif

  fp1 = fopen(legacy_f,"rb"); 
  if(!fp1){
    fprintf(stderr, "Could not open legacy file %s for reading.\n", legacy_f);
    stop = 1;
  }
  else
    fclose(fp1);

  fp2 = fopen(update_f,"rb"); 
  if(!fp2){
    fprintf(stderr, "Could not open update file %s for reading.\n", update_f);
    stop = 1;
  }
  else
    fclose(fp2);

  fp3 = fopen(delta_f,"wb"); 
  if(!fp3){
    fprintf(stderr, "Could not access/create delta script: %s.\n", delta_f);
    stop = 1;
  }
  else
    fclose(fp3);

  if(stop)
    exit(EXIT_FAILURE);

  realpath(legacy_f, abs_legacy_f);
  realpath(update_f, abs_update_f);
  realpath(delta_f, abs_delta_f);

  #if VERBOSE==1
  
  fprintf(stdout, "Legacy file: %s -> %s\n"
                  "Update file: %s -> %s\n"
                  "Delta file:  %s -> %s\n",
                  legacy_f, abs_legacy_f, update_f, abs_update_f, 
                  delta_f, abs_delta_f);

  fprintf(stdout, "Target: ");
  executables == 1 ? fprintf(stdout, "Executables\n") : fprintf(stdout, "Generic binary files\n");
  fprintf(stdout, "Reconstruction type: ");
  in_place == 1 ? fprintf(stdout, "In-place\n") : fprintf(stdout, "Out-of-place\n");
  fprintf(stdout, "Padding: ");
  padding == 1 ? fprintf(stdout, "Yes\n") : fprintf(stdout, "No\n");

  fprintf(stdout, "Operation: ");
  reconstruction == 1 ? fprintf(stdout, "Reconstruction\n") : fprintf(stdout, "Delta computation\n");
  
  #endif

  struct padded_str_s padded_ret = create_padded_str(update_f, legacy_f);

  uint* suffixArr = buildSuffixArray(padded_ret.padded_str, padded_ret.padded_length);
  uint* rankArr   = buildRankArray(suffixArr, padded_ret.padded_length); 
  uint* lcpArr    = buildLCPArray(suffixArr, rankArr, padded_ret.padded_str, padded_ret.padded_length);


    f_commands = computeCommands(padded_ret.lims, suffixArr,
                                rankArr, lcpArr, padded_ret.padded_str,
                                padded_ret.new_length, padded_ret.new_f, in_place);
  write_delta_script(delta_f, f_commands, padded_ret.new_length);


  return EXIT_SUCCESS;
}