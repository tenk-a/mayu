// Linux,Darwin�p��maine

#define APSTUDIO_INVOKED

#include "misc.h"
#include "compiler_specific_func.h"
#include "engine.h"
#include "errormessage.h"
#include "function.h"
#include "mayu.h"
#if defined(WIN32)
#include "mayurc.h"
#endif
#include "msgstream.h"
#include "multithread.h"
#include "setting.h"
#include <time.h>
#include "gcc_main.h"


#if defined(__linux__) || defined(__APPLE__)
// TODO:

int main(int argc, char* argv[])
{
  __argc = argc;
  __targv = argv;
	
  //TODO: ���d�N�� check
	
  try
  {
    Mayu mayu;

    // �ݒ�t�@�C���̃��[�h
    if (mayu.load())
    {
      //�R�}���h���s����Enter���c��\�������邽�߁A������Ƃ����҂�
      sleep(2);
		
      // �L�[�u�������̎��s
      mayu.taskLoop();			
    }
  }
  catch (ErrorMessage e)
  {
    fprintf(stderr, "%s\n", e.getMessage().c_str());
  }
	
  return 0;
}

#endif
