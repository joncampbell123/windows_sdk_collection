; copyright (C) Microsoft Corporation, 1988

;;STRUC.INC--Stucture Macro Library
if1
$noconj equ     0
$and            equ     1
$or             equ     2
$short          equ     3
$near           equ     4
$andor          =       0
$temp           =       0
$temp2          =       0
$dist           =       0

$notype         equ     10
$conjif         equ     11
$iftype         equ     12
$elsetype       equ     13
$whiletype      equ     14
$conjwhile      equ     15
$repeattype     equ     16
$conjuntil      equ     17
$fortype        equ     18
$conjleave      equ     19

$StrucErr macro text
                                        Structure error -- text
endm

j       macro
endm
jn      macro
endm

jbuild  macro   j1,j2
        j1      macro   tgt
                .xcref  j1
                                        j2 tgt
        endm
endm

irp     x,<<jeq,je>,<jlt,jl>,<jgt,jg>,<jneq,jne>,<jnlt,jnl>,<jngt,jng>,<jnpe,jpo>,<jnpo,jpe>>
        jbuild  x
endm
irp     x,<<jzero,jz>,<jnzero,jnz>,<jnonzero,jnz>,<jnnonzero,jz>,<jand,jnz>,<jnand,jz>,<jnnand,jnz>>
        jbuild  x
endm
irp     x,<a,ae,b,be,c,e,g,ge,l,le,o,p,s,z,cxz>
        jbuild  jnn&x,j&x
endm

jncxz   macro   tgt
        local   skip
                                        jcxz skip
                                        jmp short tgt
skip:
endm

purge   jbuild

$getconj macro  p1,p2
        $andor  = $noconj
        irp     parm,<p1,p2>
                ifnb    <&parm>
                        irp     x,<and,AND,or,OR>
                                ifidn   <parm>,<x>
                                        $andor = $&&&x
                                        exitm
                                endif
                        endm
                endif
        endm
endm

$getdist macro  p1,p2
        irp     parm,<p1,p2>
                ifnb    <parm>
                        irp     x,<short,SHORT,near,NEAR>
                                ifidn   <parm>,<x>
                                        $dist = $&&&x
                                        exitm
                                endif
                        endm
                endif
        endm
endm


$poke   macro   num,value
        $st&num = value
.xcref  $st&num
endm

$peek   macro   sym,num
        sym     = $st&num
endm

$push   macro   value
        $st     = $st+1
        $poke   %$st,value
endm

$pop    macro   sym
        $peek   sym,%$st
        $st     = $st-1
endm

$labl   macro   num
$l&num:
endm

$cjump  macro   lnum,tf,cond
        local   skip
        if      $dist eq $short
                ifb     <cond>
                                        jmp short $l&lnum
                else
                        ifidn <tf>,<f>
                                        jn&cond $l&lnum
                        else
                                        j&cond $l&lnum
                        endif
                endif
        else
                ifnb    <cond>
                        ifidn <tf>,<f>
                                        j&cond skip
                        else
                                        jn&cond skip
                        endif
                endif
                                        jmp $l&lnum
                ifnb            <cond>
skip:
                endif
        endif
endm


$cloop  macro   lnum,cond
                                        loop&cond $l&lnum
endm

$test   macro   tgt,a1,a2,a3,a4,x1
        ifb     <a1>
                $StrucErr <invalid condition>
        else
        ifb     <a2>
                $cjump  %&tgt,a1
        else
        ifb     <a3>
                ifdif   <a1>,<zero>
                ifdif   <a1>,<nonzero>
                ifdif   <a1>,<ZERO>
                ifdif   <a1>,<NONZERO>
                        $StrucErr       <invalid condition>
                        exitm
                endif
                endif
                endif
                endif
                                        or a2,a2
                $cjump  %&tgt,a1
        else
        ifb     <a4>
                                        cmp a1,a3
                $cjump  %&tgt,a2
        else
        ifb     <x1>
                ifdif   <a1>,<bit>
                ifdif   <a1>,<BIT>
                        $StrucErr       <invalid condition>
                        exitm
                endif
                endif
                                        test a2,a4
                $cjump  %&tgt,a3
        else
                $StrucErr       <invalid condition>
        endif
        endif
        endif
        endif
        endif
endm

$toptest macro  args,ntype,ctype,p4,p5
        $getconj p4,p5
        $dist = $defdist
        $getdist p4,p5
        if      $andor eq $noconj
                $test   <$sn-1,f>,args
                $pop    $temp
                if       $orfound
                        $labl  %$temp
                endif
                $push   ntype
        else
                if      $andor eq $and
                        $test   <$sn-1,f>,args
                else
                        $orfound = 1
                        $test   <$sn,t>,args
                endif
                $push   ctype
        endif
endm
;;*****************************************************************************
.if     macro   tst,p2,p3
        $peek   $temp,%$st
        if      $temp eq $conjif
                $pop    $temp
        else
                $push   $elseiffound
                $elseiffound = 0
                $orfound = 0
                $sn     = $sn+1
                $push   $sn
                $sn     = $sn+1
                $push   $sn
                $sn     = $sn+1
                $push   $sn
        endif
        $toptest <tst>,$iftype,$conjif,p2,p3
endm
;;*****************************************************************************
.then   macro
        $peek   $temp,%$st
        if      $temp ne $iftype
                $StrucErr <then without if>
        endif
endm
;;*****************************************************************************
.elseif macro   tst,p2,p3
        $pop    $temp
        if      $temp ne $iftype
                $StrucErr <elseif without if>
                exitm
        endif
        $elseiffound = 1
        $orfound = 0
        $pop    $temp
        $peek   $temp2,%$st
        $dist   = $near
        $cjump  %$temp2
        $labl   %$temp
        $sn     = $sn+1
        $push   $sn
        $sn     = $sn+1
        $push   $sn
        $toptest <tst>,$iftype,$conjif,p2,p3
endm
;;*****************************************************************************
.else   macro   dist
        $pop    $temp
        if      $temp ne $iftype
                $StrucErr <else without if>
                exitm
        endif
        $sn     = $sn+1
        $dist   = $defdist
        $getdist dist
        $cjump  %$sn
        $pop    $temp
        $labl   %$temp
        $push   $sn
        $push   $elsetype
endm
;;*****************************************************************************
.endif  macro
        $pop    $temp
        if      $temp ne $iftype
                if      $temp ne $elsetype
                        $StrucErr <endif without if>
                        exitm
                endif
        endif
        $pop    $temp
        $labl   %$temp
        $pop    $temp
        if      $elseiffound
                $labl    %$temp
        endif
        $pop    $elseiffound
endm
;;*****************************************************************************
.while  macro   tst,p2,p3
        $peek   $temp,%$st
        if      $temp eq $conjwhile
                $pop            $temp
        else
                $push $endloop
                $orfound = 0
                $sn = $sn + 1
                $push $sn
                $labl %$sn
                $sn = $sn + 2
                $push $sn
                $endloop = $sn - 1
        endif
        $toptest <tst>,$whiletype,$conjwhile,p2,p3
endm
;;*****************************************************************************
.endwhile macro
        $pop    $temp
        if      $temp ne $whiletype
                $StrucErr <endwhile without while>
                exitm
        endif
        $pop    $temp
        $dist   = $near
        $cjump  %$temp
        $labl   %$temp+1
        $pop    $endloop
endm
;;*****************************************************************************
.repeat macro
        $push   $endloop
        $push   $leavefound
        $sn     = $sn+1
        $labl   %$sn
        $push   $sn
        $push   $repeattype
        $sn     = $sn+1
        $endloop = $sn
        $leavefound = 0
endm
;;*****************************************************************************
.until  macro   tst,p2,p3
        $until_2 p2,p3,tst
endm
$until_2 macro  p2,p3,a1,a2,a3,a4,x1
        $pop    $temp
        if      $temp ne $repeattype
                if      $temp ne $conjuntil
                        $StrucErr <until without repeat>
                        exitm
                endif
        else
                $orfound = 0
        endif
        $dist   = $defdist
        $getdist p2,p3
        $getconj p2,p3

        if      $andor eq $noconj
                $pop    $temp
                ifb     <a1>
                        $dist   = $near
                        $cjump  %$temp,f
                else
                        $test   <$temp,f>,<a1>,<a2>,<a3>,<a4>,<x1>
                endif
                if      $orfound or $leavefound
                        $labl %$temp+1
                endif
                $pop    $leavefound
                $pop    $endloop
        else
                $peek $temp,%$st
                if      $andor eq $and
                        $test <$temp,f>,<a1>,<a2>,<a3>,<a4>,<x1>
                else
                        $orfound = 1
                        $test <$temp+1,t>,<a1>,<a2>,<a3>,<a4>,<x1>
                endif
                $push $conjuntil
        endif
endm
;;*****************************************************************************
.loop   macro   cond
        $pop    $temp
        if      $temp ne $repeattype
                $StrucErr <loop without repeat>
                exitm
        endif
        $pop    $temp
        $cloop  %$temp,cond
        if      $leavefound
                $labl %$temp+1
        endif
        $pop    $leavefound
        $pop    $endloop
endm
;;*****************************************************************************
.for    macro   index,equals,start,to,stop,by,step,dist
                                        mov index,start
        $push   $endloop
        $sn     = $sn+1
        $push   $sn
        $labl   %$sn
        $sn     = $sn+1
        $endloop = $sn
                                        cmp index,stop
        $dist   = $defdist
        ifb     <step>
                $push 1
                $getdist by
                $cjump %$sn,t,gt
        else
                $getdist dist
                $push %(step)
                if      step lt 0
                        $cjump %$sn,t,lt
                else
                        $cjump %$sn,t,gt
                endif
        endif
        $push   $fortype
endm
;;*****************************************************************************
.next   macro   index
        $pop    $temp
        if      $temp ne $fortype
                $StrucErr <next without for>
                exitm
        endif
        $pop    $temp
        if      $temp eq 1
                                        inc index
        else
                if      $temp eq -1
                                        dec index
                else
                                        add index,$temp
                endif
        endif
        $pop    $temp
        $dist   = $near
        $cjump  %$temp
        $labl   %$temp+1
        $pop    $endloop
endm
;;*****************************************************************************
.leave  macro   tst,p2,p3
        $leave_2 p2,p3,tst
endm
$leave_2 macro  p2,p3,a1,a2,a3,a4,x1
        ife     $endloop
                $StrucErr <leave outside a loop>
                exitm
        endif
        $leavefound = 1
        $peek   $temp,%$st
        if      $temp eq $conjleave
                $pop            $temp
        else
                $orfound = 0
                $sn = $sn + 1
        endif
        $dist   = 0
        $getdist <a1>
        if      $dist
                $cjump %$endloop
                if      $orfound
                        $labl %$sn
                endif
        else
                $dist   = $defdist
                $getdist p2,p3
                $getconj p2,p3

                if      $andor eq $noconj
                        ifb     <a1>
                                $cjump  %$endloop,t
                        else
                                $test <$endloop,t>,<a1>,<a2>,<a3>,<a4>,<x1>
                        endif
                        if      $orfound
                                $labl %$sn
                        endif
                else
                        if      $andor eq $and
                                $orfound = 1
                                $test  <$sn,f>,<a1>,<a2>,<a3>,<a4>,<x1>
                        else
                                $test  <$endloop,t>,<a1>,<a2>,<a3>,<a4>,<x1>
                        endif
                        $push $conjleave
                endif
        endif
endm
;;*****************************************************************************
else
        $pop     $temp
        if       $temp ne $notype
                 $StrucErr <open structure(s)>
        endif
.xcref $noconj,$and,$or,$short,$near,$andor,$temp,$temp2,$dist
.xcref $notype,$conjif,$iftype,$elsetype,$whiletype,$conjwhile
.xcref $repeattype,$conjuntil,$fortype,$conjleave,jncxz
.xcref jeq,jgt,jlt,jneq,jngt,jnlt,jnna,jnnae,jnnb,jnnbe,jnnc,jnncxz
.xcref jnne,jnng,jnnge,jnnl,jnnle,jnno,jnnp,jnns,jnnz,jnpe,jnpo,jbuild
.xcref $getconj,$getdist,$poke,$peek,$push,$pop,$labl,$cjump,$cloop,$test
.xcref $toptest,$leave_2,$until_2,$strucerr,j,jn,jand,jnand,jnnand
.xcref jnnonzero,jnonzero,jnzero,jzero
.xcref $st,$sn,$orfound,$elseiffound,$endloop,$leavefound,$defdist
endif
$st             = 0
$sn             = 0
$orfound        = 0
$elseiffound    = 0
$endloop        = 0
$leavefound     = 0
$defdist        = $short
$push   %$notype
