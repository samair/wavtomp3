#include <lame/lame.h>
#include <dirent.h>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <vector>
#include "threadPool.h"
using namespace std;


#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

void encode(string);
class convertTask: public Task{

public:
  string fileName;
  void run()
  {
    encode(fileName);
  }
};


bool listWavFiles(string fileName, string dirName, ThreadPool* tPool){

  regex wavfile(".{1,}\\.wav");
  if (regex_match(fileName,wavfile))
  {
    cout<<"############### Found a wav file[ "<<fileName<<" ] encoding it to mp3"<<endl;
    convertTask t ;
    t.fileName = dirName+fileName;
    tPool->addTask(&t);
     //encode(dirName+fileName);
  }
  //create a regex for *.wav
}
void listDirs(string dirName, ThreadPool* tPool){

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (dirName.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      if((strcmp(ent->d_name,"..") != 0) && (strcmp(ent->d_name,".") != 0)){


        if(ent->d_type == DT_DIR){
    //      cout<<"Got a directory : "<<dirName+ent->d_name<<endl;
          listDirs(dirName+ent->d_name+SEPARATOR,tPool);
        }
        listWavFiles(ent->d_name,dirName,tPool);
      }

    }
    closedir (dir);
  } else {
    /* could not open directory */
    cout<<"Error opening direcotry"<<endl;

  }
}
void encode(string wavFile)
{
  int read, write;

  FILE *pcm = fopen(wavFile.c_str(), "rb");
  std::size_t found = wavFile.rfind(".wav");

  string mp3file = wavFile.substr(0,found)+".mp3";
  FILE *mp3 = fopen(mp3file.c_str(), "wb");

  const int PCM_SIZE = 8192;
  const int MP3_SIZE = 8192;

  short int pcm_buffer[PCM_SIZE*2];
  unsigned char mp3_buffer[MP3_SIZE];

  lame_t lame = lame_init();
  //lame_set_in_samplerate(lame, 44100);
  //lame_set_VBR(lame, vbr_default);
  lame_init_params(lame);

  do {
      read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
      if (read == 0)
          write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
      else if (read > 0) 
          write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
      else
	 break;
	

	 
      fwrite(mp3_buffer, write, 1, mp3);
  } while (read != 0);
  if (read >=0 && read!= 0)
  	cout<<"Encoding Finished for [ "<< wavFile<<" ]"<<endl;
  else 
  	cout<<"Encoding Error for [ "<< wavFile<<" ]"<<endl;

  lame_close(lame);
  fclose(mp3);
  fclose(pcm);
}
int main(int argc, char *argv[])
{
  if ( argc != 2 )
      // We print argv[0] assuming it is the program name
      cout<<"usage: "<< argv[0] <<" <filename>\n";
    else {
    ThreadPool* tPool = new  ThreadPool(5);
    listDirs(argv[1],tPool);
    tPool->stop();
    delete tPool;
    }


    return 0;
}
