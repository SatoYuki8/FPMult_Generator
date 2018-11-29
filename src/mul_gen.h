#ifndef HEAD_DEC
#define HEAD_DEC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ERROR 0
#define GET_VAR_NAME(VAR) (#VAR) //変数VARの変数名を文字列として取得

/* exponation fraction bit_width */
typedef struct exp_frac_width{
  int exp;
  int frac;
  int width;  //exp + frac + sign
} _efw_t;

typedef _efw_t efw_t[1];

/* Flag */
typedef struct flags{
  int wrapper_flag;
  int fpmulti_flag;
  int stdout_flag;
  int filename_flag;
  int non_DSP_flag;
  int pipeline_flag;
  int step1_flag;
  int step2_flag;
  int step3_flag;
  int step4_flag;
} _flags_t;

typedef _flags_t flags_t[1];


#define P_IF if(flag->pipeline_flag == 1){
#define P_ELSE }else{
#define P_END }

//step_measurement.c
int step_measurement(FILE *fp, const efw_t f, flags_t flag, char *module);

//main.c
void init_flags(flags_t flag);
void help_message();
int Wrapper_Generator(FILE *fp, const efw_t f, char *module_name);
int Manage_FPMult_Generator(int argc, char **argv);

FILE *arg_check(int argc, char **argv, efw_t f, flags_t flag, char *module_name);
int IncFrac_Generator(FILE *fp, const int frac, char *name);
int Long_Division(FILE *fp, const int frac, char *name);
int Multiplier_Generator(FILE *fp, const int frac, flags_t flag, char *name);
int FPMultiplier_Generator(FILE *fp, const efw_t f, flags_t flag, char *module_name);

int fpmulti_sel_declaration(FILE *fp, int exp, int frac, int width, flags_t flag, char *sel_or_reg);
int fpmulti_signxor_expadd(FILE *fp, int exp, int frac, int width, flags_t flag);
int fpmulti_fracmult(FILE *fp, flags_t flag);
int fpmulti_normalize(FILE *fp, int exp, int frac, flags_t flag);
int fpmulti_round(FILE *fp, int frac, flags_t flag);


#endif
