#ifndef DU3_LICENCE_H
//#if 1
#define DU3_LICENCE_H


//Copyright (c) <YEAR>, <OWNER> All rights reserved.\r\n\r\n 
#define BSD_LICENCE_TEMPLATE_BEGIN "Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\r\n \
.Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. \r\n \
.Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. \r\n \
.Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. \r\n"

#define BSD_LICENCE_TEMPLATE_END "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\r\n\r\n\0"



///日本ではあまりうるさくないので暗黙的にAll rights reservedなのだが、
#define DKUTIL_C_COPYRIGHT_JAPANESE "dkutil_c Copyright (c) 2003-, by d.Kingyo(d金魚) All rights reserved.\r\n"
///海外はうるさいので明確にしておく もっと上手い表記方法があったらどうか教えてください。
#define DKUTIL_C_COPYRIGHT_CONSTRUCT "dkutil_c Copyright (c) 2003-, by d.Kingyo  Construct by d.Kingyo source code and data rights reserved.\r\n"
///でもやっぱなんか言われると嫌なので統一orz
#define DKUTIL_C_COPYRIGHT "dkutil_c Copyright (c) 2003-, by d.Kingyo All rights reserved.\r\n"
#define DKUTP_URL	"http://dkingyoutility.sourceforge.jp \r\n"


#define MERSENNE_TWISTER_COPYRIGHT "mt19937ar-cok.c mt19937ar.c Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,All rights reserved.\r\n"
#define MD5_COPYRIGHT "RSA Data Security, Inc. MD5 Message-Digest Algorithm \r\n"


#define TEST_COPYRIGHT \
	DKUTP_URL\
	DKUTIL_C_COPYRIGHT \
	MD5_COPYRIGHT

#define BSD_LICENCE_LAST "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,INCLUDING, BUT NOT LIMITED TO,THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\r\n"

 #define BSD_LICENCE_BEGIN "Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\r\n \
･Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. \r\n \
･Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\r\n \
･Neither the name of the dKingyo Utility Toolkit Project nor the names of \
 its contributors may be used to endorse or promote products derived from \
 this software without specific prior written permission. \r\n " 

#define DKINGYOUTILITY3_COPYRIGHT(r) \
	DKUTP_URL "\r\n" r "dkutil_c : BSD Licence\r\n" "\r\n"  MD5_COPYRIGHT "\0"
	
#define BSD_LICENCE_PUT1\
	DKUTIL_C_COPYRIGHT "<ORGANIZATION> = dKingyo Utility Toolkit Project" "\r\n\r\n" \
	MERSENNE_TWISTER_COPYRIGHT "\r\n\r\n" \
	 "Goto [BSD Licence 2] section"
	 


#define	BSD_LICENCE_PUT2 BSD_LICENCE_TEMPLATE_BEGIN "\r\n" "Goto [BSD Licence 3] section"

#define BSD_LICENCE_PUT3 BSD_LICENCE_TEMPLATE_END "\r\n"



#endif


