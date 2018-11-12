#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "generator.h"

int step_measurement(FILE *fp, const efw_t f, flags_t flag, char *module){
  int exp = f->exp;
  int frac= f->frac;
  int width=f->width;
  char sub_module[64];

  if (flag->step1_flag == 1){
    fprintf(fp,
	    "circuit %s{\n"
	    "input a<%d>, b<%d>;\n"
	    "output  As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "instrin do;\n"
	    "sel xmsb, ymsb;\n\n"
	    "\n",
	    module,
	    width, width,
	    exp+1, frac+1, frac+1
	    );

    fprintf(fp,
	    "instruct do par{\n"
	    "alt{\n"
	    "(a<%d:0> == %d#(0b0)): xmsb = 0b0;\n"
	    "else: xmsb = 0b1;\n"
	    "}\n\n",
	    width-2, width-1
	    );
    fprintf(fp,
	    "alt{\n"
	    "(b<%d:0> == %d#(0b0)): ymsb = 0b0;\n"
	    "else: ymsb = 0b1;\n"
	    "}\n\n",
	    width-2, width-1
	    );
    fprintf(fp, "As = a<%d>@b<%d>;\n", width-1, width-1);
    fprintf(fp, "Ae = (0b0||a<%d:%d>) + (0b0||b<%d:%d>) + (^(0b0||(%d#0b1)) + 0b1);\n",
	    width-2, frac, width-2, frac, exp-1);
    fprintf(fp,
	    "Am1 = xmsb||a<%d:0>;\n"
	    "Am2 = ymsb||b<%d:0>;\n"
	    "}\n"
	    "}\n"
	    "\n",
	    frac-1, frac-1
	    );
    fprintf(fp,
	    "declare step1{\n"
	    "input a<%d>, b<%d>;\n"
	    "instrin do;\n"
	    "output As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "instr_arg do(a, b);\n"
	    "}\n"
	    "module wrapper{\n"
	    "   input a<%d>, b<%d>;\n"
	    "  instrin do;\n"
	    " output As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "step1 step;\n"
	    "reg_wr ina<%d>, inb<%d>;\n"
	    "reg_wr as, ae<%d>, am1<%d>, am2<%d>;\n"
	  
	    "instruct do par{\n"
	    "	ina := a;\n"
	    "	inb := b;\n"
	    "	step.do(ina, inb);\n"
	    "	as :=step.As;\n"
	    "	ae := step.Ae;\n"
	    "	am1:=step.Am1;\n"
	    "	am2:=step.Am2;\n"
	    "	As = as;\n"
	    "	Ae = ae;\n"
	    "	Am1= am1;\n"
	    "	Am2= am2;\n"
	    "   }\n"
	    "}\n",
	    width, width,
	    exp+1, frac+1, frac+1,
	    width, width,
	    exp+1, frac+1, frac+1,
	    width, width,
	    exp+1, frac+1, frac+1	  
	    );
  }else if(flag->step2_flag == 1){
    fprintf(fp,
	    "circuit step2{\n"
	    "    instrin do;\n"
	    
	    "    input As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    	    
	    "    instruct do par{	\n"
	    "	Bs = As;\n"
	    "	Be = Ae;\n"
	    "	Bm = Am1 * Am2;\n"
	    "    }\n"
	    "}\n"
	    
	    "declare step2{\n"
	    "    instrin do;\n"
	    
	    "    input As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    
	    "    instr_arg do(As, Ae, Am1, Am2);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    input As, Ae<%d>, Am1<%d>, Am2<%d>;\n"
	    "    output Bs, Be<%d>, Bm<%d>;\n"
	    "    instrin do;\n"
	    
	    "    step2 step;\n"
	    
	    "    reg_wr as, ae<%d>, am1<%d>, am2<%d>;\n"
	    "    reg_wr bs, be<%d>, bm<%d>;\n"
	    
	    "    instruct do par{\n"
	    "	as := As;\n"
	    "	ae := Ae;\n"
	    "	am1:= Am1;\n"
	    "	am2:= Am2;\n"
	    "	step.do(as, ae, am1, am2);\n"
	    "	bs := step.Bs;\n"
	    "	be := step.Be;\n"
	    "	bm := step.Bm;\n"
	    "	Bs = bs;\n"
	    "	Be = be;\n"
	    "	Bm = bm;\n"
	    "    }\n"
	    "}\n"
	    "\n",
	    exp+1, frac+1, frac+1,
	    exp+1, (frac+1)*2,
	    exp+1, frac+1, frac+1,
	    exp+1, (frac+1)*2,
	    exp+1, frac+1, frac+1,
	    exp+1, (frac+1)*2,
	    exp+1, frac+1, frac+1,
	    exp+1, (frac+1)*2

	    );
    
  }else if(flag->step3_flag == 1){
    fprintf(fp,
	    "circuit step3{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>, carry;\n"

	    "    instruct do	par{\n"
	    "	Cs = Bs;\n"
	    "	Cexp = Bexp;\n"
	    "	Cm = Bm >> Bm<%d>;\n"
	    "	carry = Bm<%d>;\n"
	    "    }\n"
	    "}\n"


	    "declare step3{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>, carry;\n"

	    "    instr_arg do(Bs, Bexp, Bm);\n"
	    "}\n"

	    "module wrapper{\n"
	    "    instrin do;\n"

	    "    input Bs, Bexp<%d>, Bm<%d>;\n"
	    "    output Cs, Cexp<%d>, Cm<%d>, carry;\n"

	    "    step3 step;\n"

	    "    reg_wr bs, bexp<%d>, bm<%d>;\n"
	    "    reg_wr cs, cexp<%d>, cm<%d>, cry;\n"

	    "    instruct do par{\n"
	    "	bs:=Bs;\n"
	    "	bexp:=Bexp;\n"
	    "	bm:=Bm;\n"
	    "	step.do(bs, bexp, bm);\n"
	    "	cs:=step.Cs;\n"
	    "	cexp:=step.Cexp;\n"
	    "	cm:=step.Cm;\n"
	    "	cry:=step.carry;\n"
	    "	Cs=cs;\n"
	    "	Cexp=cexp;\n"
	    "	Cm=cm;\n"
	    "	carry=cry;\n"
	    "    }\n"
	    "}\n",
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    (frac+1)*2-1,
	    (frac+1)*2-1,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2
	    );
  }else if(flag->step4_flag == 1){
    fprintf(fp,
	    "declare IncreaseFrac_%d{\n"
	    "    input cin;\n"
	    "    input in<%d>;\n"
	    "    output out<%d>;\n"
	    "    output p;\n"
	    "    instrin do;\n"
	    "    instr_arg do(cin, in);\n"
	    "}\n",
	    frac, frac, frac
	    );
    fprintf(fp,
	    "circuit IncreaseFrac_%d{\n"
	    "    input cin;\n"
	    "    input in<%d>;\n"
	    "    output out<%d>;\n"
	    "    output p;\n"
	    "    instrin do;\n"
	    "    sel tmp<%d>;\n"
	    "    instruct do par{\n"
	    "        tmp = (0b0||in) + cin;\n"
	    "        out = tmp<%d:0>;\n"
	    "        p = tmp<%d>;\n"
	    "    }\n"
	    "}\n",
	    frac, frac, frac, frac+1,
	    frac-1, frac
	    );
    fprintf(fp,
	    "circuit step4{\n"
	    "    instrin do;\n"
	    
	    "    IncreaseFrac_%d incfrac;\n"
	    
	    "    sel rounded_frac<%d>;\n"
	    
	    "    input Cs, Cexp<%d>, Cm<%d>, carry;\n"
	    "    output out<%d>;\n"
	    "    instruct do par{\n",
	    frac,
	    frac,
	    exp+1, (frac+1)*2,
	    width
	    );
    fprintf(fp,
	    "alt{\n"
	    "(^(/|Cm)): out = (%d#(0b0));\n"
	    "else: par{\n",
	    width
	    );
    
    fprintf(fp,
	    "rounded_frac = incfrac.do((Cm<%d>&(Cm<%d>|Cm<%d>|(/|Cm<%d:0>))), Cm<%d:%d>).out;\n",
	    frac-1, frac, frac-2, frac-3, ((frac+1)*2-3), frac
	    );
    fprintf(fp,
	    "out = (Cs || (Cexp<%d:0> + carry + incfrac.p) || rounded_frac);\n"
	    "}\n"
	    "}\n"
	    "}\n"
	    "}\n",
	    exp-1
	    );
    fprintf(fp,
	    "declare step4{\n"
	    "    instrin do;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>, carry;\n"
	    "    output out<64>;\n"
	    "    instr_arg do(Cs, Cexp, Cm, carry);\n"
	    "}\n"
	    
	    "module wrapper{\n"
	    "    instrin do;\n"
	    "    input Cs, Cexp<%d>, Cm<%d>, carry;\n"
	    "    output out<64>;\n"

	    "    step4 step;\n"
	    
	    "    reg_wr cs, cexp<%d>, cm<%d>, cry;\n"
	    "    reg_wr z<64>;\n"
	    
	    "    instruct do par{\n"
	    "	cs:=Cs;\n"
	    "	cexp:=Cexp;\n"
	    "	cm:=Cm;\n"
	    "	cry:=carry;\n"
	    "	z:=step.do(cs, cexp, cm, cry).out;\n"
	    "	out = z;\n"
	    "    }\n"
	    "}\n",
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2,
	    exp+1, (frac+1)*2	    
	    );
  }
  
  return 0;
}
