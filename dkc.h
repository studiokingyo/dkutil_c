/*!
@file dkc.h
@author d����
@since 2004/03/03
@brief ���̃��C�u�������g���ꍇ�͂�����C���N���[�h����ׂ�
@note
<h1>dkutil_c�̂��������g����</h2>

��`�S
uint8 / 8bit��unsigned�^
uint16 / 16bit��unsigned�^
uint32 / 32bit��unsigned�^

ULONGLONG / 64bit��unsigned�^
LONGLONG / 64bit��signed�^
uint64 / ULONGLONG�Ɠ���

-# �o�C�g�I�[�_�[�̐ݒ�
����Ƀo�C�g�I�[�_�[��ݒ肵�Ă���܂��B

ORDER_DCBA��little endian

-# ���[�U�[���ݒ�ł��鎖

#define DKUTIL_C_USE_DKUTIL_CSTD
������`����ƁA���O��C�����^�C�����C�u����(�s���S)���g���悤�ɂȂ�܂��B
(�܂��A���������)



#define DKUTIL_C_2TREE_FAST_IMPL
dkc2Tree.c dkc2Tree.h�̓񕪖؂̈ꕔ�̏����̑������������i�ăR���p�C����L���j
�ڂ�����dkc2Tree.h�ɂāB

#define DKUTIL_C_USE_BIT_TABLE
�����1�Ƃ��Ē�`����ƁA�r�b�g���Z�e�[�u�����g���悤�ɂȂ�܂��B
�Ⴆ�΁A(1 << n)��table32[n] �̗l�ɂ���Ƃ��B
*/

#ifndef DKUTIL_C_DKC_H
#define DKUTIL_C_DKC_H

/* generic */
#include "dkcDefined.h"
#include "dkcOSIndependent.h"
#include "dkcStdio.h"

/* container */
#include "dkcString.h"

#include "dkcSingleList.h"
#include "dkcStack.h"
#include "dkcQueue.h"
#include "dkcDeque.h"
#include "dkc2Tree.h"
#include "dkcBTree.h"
#include "dkcRedBlackTree.h"
#include "dkcMemoryStream.h"

#include "dkcCircularMemoryStream.h"
#include "dkcArrayOneByOne.h"
#include "dkcHashSet.h"
#include "dkcHashMap.h"
#include "dkcHashMultiSet.h"
#include "dkcHashMultiMap.h"
#include "dkcDoubleList.h"
#include "dkcNaryTree.h"

/* generic interface */
#include "dkcCircularStream.h"
#include "dkcStream.h"





/* file system */

#include "dkcFileSystems.h"
/* utility */

#include "dkcDLL.h"
#include "dkcThread.h"
#include "dkcThreadLock.h"
#include "dkcMemoryPool.h"
#include "dkcMath.h"

/* algorithm */
#include "dkcSort.h"
#include "dkcMath.h"
#include "dkcUniqueID.h"
#include "dkcRegex.h"

/* compress */
#include "dkcLZSS.h"
#include "dkcRLE.h"
#include "dkcLZW.h"

/* cryptograph */
#include "dkcCryptoGraph.h"
#include "dkcBase64.h"


/* gemerate signature */
#include "dkcHash.h" /* and hash table */
#include "dkcCRC.h"

/* routine */
#include "dkcTelecomConfirmPacket.h"
#include "dkcTelecomConfirmFile.h"
#include "dkcDCF.h"


/* os dependence */
#include "dkcOS.h"




#ifndef DKINGYOUTILITY3_H

#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread
//#			pragma message("import dkutil_cMTd")
#			pragma comment(lib,"dkutil_cMTd")
#		else
//#			pragma message("import dkutil_cd")
#			pragma comment(lib,"dkutil_cd")
#		endif
#	else
#		ifdef dkcdMultiThread
//#			pragma message("import dkutil_cMT")
#			pragma comment(lib,"dkutil_cMT")
#		else
//#			pragma message("import dkutil_c")
#			pragma comment(lib,"dkutil_c")
#		endif
#	endif
#endif

#endif
	
#endif
