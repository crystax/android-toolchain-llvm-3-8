; RUN: llc < %s -mtriple=i386-pc-linux-gnu -x86-force-gv-stack-cookie -o - \
; RUN:   | FileCheck %s

@.str = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
declare i8* @strcpy(i8*, i8*)
declare i32 @printf(i8*, ...)

define void @test(i8* %a) nounwind uwtable ssp {
entry:
; CHECK: test:
; CHECK: movl __stack_chk_guard
; CHECK: calll __stack_chk_fail
  %a.addr = alloca i8*, align 8
  %buf = alloca [16 x i8], align 16
  store i8* %a, i8** %a.addr, align 8
  %arraydecay = getelementptr inbounds [16 x i8]* %buf, i32 0, i32 0
  %0 = load i8** %a.addr, align 8
  %call = call i8* @strcpy(i8* %arraydecay, i8* %0)
  %arraydecay1 = getelementptr inbounds [16 x i8]* %buf, i32 0, i32 0
  %call2 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), i8* %arraydecay1)
  ret void
}
