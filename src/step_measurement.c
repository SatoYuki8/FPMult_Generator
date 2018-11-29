#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mul_gen.h"

int step_measurement(FILE *fp, const efw_t f, flags_t flag, char *module){
  int exp = f->exp;
  int frac= f->frac;
  int width=f->width;
  char sub_module[64];
  flag->pipeline_flag = 0;

  if (flag->step1_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "input a<%d>, b<%d>;\n"
	    "output  As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "instrin do;\n"
	    "sel xmsb, ymsb;\n\n"
	    "sel added_exp<%d>;\n"
	    "\n",
	    module,
	    width, width,
	    exp, frac+1, frac+1,
	    exp+1
	    );

    fprintf(fp,
	    "instruct do par{\n"
	    );

    fpmulti_signxor_expadd(fp, exp, frac, width, flag);
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    
    fprintf(fp,
	    "declare %s{\n"
	    "input a<%d>, b<%d>;\n"
	    "instrin do;\n"
	    "output As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "instr_arg do(a, b);\n"
	    "}\n"
	    "module wrapper{\n"
	    "   input a<%d>, b<%d>;\n"
	    "  instrin do;\n"
	    " output As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "%s step;\n"
	    "reg_wr ina<%d>, inb<%d>;\n"
	    "reg_wr as, aexp<%d>, am1<%d>, am2<%d>;\n"
	  
	    "instruct do par{\n"
	    "	ina := a;\n"
	    "	inb := b;\n"
	    "	step.do(ina, inb);\n"
	    "	as :=step.As;\n"
	    "	aexp := step.Aexp;\n"
	    "	am1:=step.Am1;\n"
	    "	am2:=step.Am2;\n"
	    "	As = as;\n"
	    "	Aexp = aexp;\n"
	    "	Am1= am1;\n"
	    "	Am2= am2;\n"
	    "   }\n"
	    "}\n",
	    module,
	    width, width,
	    exp, frac+1, frac+1,
	    width, width,
	    exp, frac+1, frac+1,
	    module,
	    width, width,
	    exp, frac+1, frac+1	  
	    );
  }else if(flag->step2_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do;\n"
	    
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    	    
	    "    instruct do par{	\n"
	    "	Bs = As;\n"
	    "	Be = Aexp;\n"
	    "	Bm = Am1 * Am2;\n"
	    "    }\n"
	    "}\n"
	    
	    "declare %s{\n"
	    "    instrin do;\n"
	    
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    
	    "    instr_arg do(As, Aexp, Am1, Am2);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    input As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    "    instrin do;\n"
	    
	    "    %s step;\n"
	    
	    "    reg_wr as, aexp<%d>, am1<%d>, am2<%d>;\n"
	    "    reg_wr bs, be<%d>, bm<%d>;\n"
	    
	    "    instruct do par{\n"
	    "	as := As;\n"
	    "	aexp := Aexp;\n"
	    "	am1:= Am1;\n"
	    "	am2:= Am2;\n"
	    "	step.do(as, aexp, am1, am2);\n"
	    "	bs := step.Bs;\n"
	    "	be := step.Be;\n"
	    "	bm := step.Bm;\n"
	    "	Bs = bs;\n"
	    "	Be = be;\n"
	    "	Bm = bm;\n"
	    "    }\n"
	    "}\n"
	    "\n",
	    module,
	    exp, frac+1, frac+1,
	    exp, (frac+1)*2,
	    module,
	    exp, frac+1, frac+1,
	    exp, (frac+1)*2,
	    exp, frac+1, frac+1,
	    exp, (frac+1)*2,
	    module,
	    exp, frac+1, frac+1,
	    exp, (frac+1)*2

	    );
    
  }else if(flag->step3_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    sel normalized_frac<%d>;\n"
	    "    sel sticky;\n"
	    "    instruct do	par{\n",	    
	    module,
	    exp, (frac+1)*2,
	    exp, frac+3,
	    (frac+1)*2
	    );
    
    fpmulti_normalize(fp, exp, frac, flag);
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    
    fprintf(fp,
	    "declare %s{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    instr_arg do(Bs, Bexp, Bm);\n"
	    "}\n"

	    "module wrapper{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>;\n"

	    "    %s step;\n"

	    "    reg_wr bs, bexp<%d>, bm<%d>;\n"
	    "    reg_wr cs, cexp<%d>, cm<%d>;\n"

	    "    instruct do par{\n"
	    "	bs:=Bs;\n"
	    "	bexp:=Bexp;\n"
	    "	bm:=Bm;\n"
	    "	step.do(bs, bexp, bm);\n"
	    "	cs:=step.Cs;\n"
	    "	cexp:=step.Cexp;\n"
	    "	cm:=step.Cm;\n"
	    "	Cs=cs;\n"
	    "	Cexp=cexp;\n"
	    "	Cm=cm;\n"
	    "    }\n"
	    "}\n",
	    module,
	    exp, (frac+1)*2,
	    exp, frac+3,
	    exp, (frac+1)*2,
	    exp, frac+3,
	    module,
	    exp, (frac+1)*2,
	    exp, frac+3
	    );
    
  }else if(flag->step4_flag == 1){
    char incfrac[64];
    
    IncFrac_Generator(fp, frac, incfrac);
    fprintf(fp,
	    "circuit %s{\n"
	    "    instrin do;\n"
	    
	    "    %s incfrac;\n"
	    
	    "    sel rounded_frac<%d>;\n"
	    
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"
	    "    instruct do par{\n",
	    module,
	    incfrac,
	    frac,
	    exp+1, frac+3,
	    width
	    );

    fpmulti_round(fp, frac, flag);
    
    fprintf(fp,
	    "}\n"
	    "}\n"
	    );
    fprintf(fp,
	    "declare %s{\n"
	    "    instrin do;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"
	    "    instr_arg do(Cs, Cexp, Cm);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    instrin do;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>;\n"
	    "    output out<%d>;\n"

	    "    %s step;\n"
	    
	    "    reg_wr cs, cexp<%d>, cm<%d>;\n"
	    "    reg_wr z<%d>;\n"
	    
	    "    instruct do par{\n"
	    "	cs:=Cs;\n"
	    "	cexp:=Cexp;\n"
	    "	cm:=Cm;\n"
	    "	z:=step.do(cs, cexp, cm).out;\n"
	    "	out = z;\n"
	    "    }\n"
	    "}\n",
	    module,
	    exp, frac+3,
	    width,
	    exp, frac+3,
	    width,
	    module,
	    exp, frac+3,
	    width
	    );
  }

  fprintf(stdout,
	  "Generate %s module\n",
	  module
	  );
  
  return 0;
}
