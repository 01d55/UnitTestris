#include <AL/alc.h>
#include <AL/al.h>
#include <vorbis/vorbisfile.h>

#include <iostream>
#include <vector>

#include "music.hpp"

struct alstate
{
  ALCdevice * dev;
  ALCcontext * ctx;

  ALuint bufferName,sourceName;

  bool playing;

} AL_STATE;

///

static bool init_al();
static bool init_ctx();

static void ov_errcode(int);

static void terminate_ctx();
static void terminate_al();

static bool read_oggv();

///

bool init_music()
{
  bool ret;
  ret=init_ctx();
  if(!ret)
    {
      return ret;
    }
  ret=init_al();
  if(!ret)
    {
      terminate_ctx();
      return ret;
    }
  /*
  ret=read_oggv();
  if(!ret)
    {
      terminate_music();
      return ret;
    }
  */
  return ret;
}


void reset_music()
{
  alSourceRewind(AL_STATE.sourceName);
  AL_STATE.playing = false;
}


void terminate_music()
{
  terminate_al();
  terminate_ctx();
}
// Play/pause
void toggle_play_music()
{
  if(AL_STATE.playing)
    {
      alSourcePause(AL_STATE.sourceName);
    }
  else
    {
      alSourcePlay(AL_STATE.sourceName);
    }
  AL_STATE.playing = !AL_STATE.playing;
}

///

static bool init_ctx()
{
  AL_STATE.dev = alcOpenDevice(nullptr);
  if (nullptr==AL_STATE.dev)
    {
      std::cerr << "Failed to open default device.\n";
      return false;
    }

  AL_STATE.ctx = alcCreateContext(AL_STATE.dev,nullptr);
  if(nullptr==AL_STATE.ctx)
    {
      std::cerr << "Failed to create AL context\n";
      return false;
    }

  if(ALC_FALSE==alcMakeContextCurrent(AL_STATE.ctx))
    {
      std::cerr << "Failed to make AL context current\n";
      return false;
    }

  alcProcessContext(AL_STATE.ctx);
  return true;
}

static bool init_al()
{
  alGenBuffers(1,&AL_STATE.bufferName);
  alGenSources(1,&AL_STATE.sourceName);
  
  if(!read_oggv())
    {
      terminate_al();
      return false;
    }
  
  alSourcei(AL_STATE.sourceName, AL_BUFFER, AL_STATE.bufferName);
  alSourcei(AL_STATE.sourceName, AL_LOOPING, AL_TRUE);
  if(AL_NO_ERROR!=alGetError())
    {
      std::cerr << "AL error during AL init\n";
      return false;
    }

  AL_STATE.playing=false;

  return true;
}

static void terminate_al()
{
  alDeleteSources(1,&AL_STATE.sourceName);
  alDeleteBuffers(1,&AL_STATE.bufferName);
}

static void terminate_ctx()
{
  alcDestroyContext(AL_STATE.ctx);
  alcCloseDevice(AL_STATE.dev);
}

static bool read_oggv()
{
  constexpr size_t HUNKSIZE=32*(1<<10);
  constexpr char FNAME[]="sound/music.ogg";
  constexpr bool bigEndian = false;
  constexpr int endianness = bigEndian? 1:0;

  int retcode,bitStream=0;
  long bytes;

  ALenum format;
  ALsizei freq;

  vorbis_info *info;
  OggVorbis_File oggFile;

  char hunk[HUNKSIZE];
  std::vector<char> databuff;

  retcode = ov_fopen(FNAME,&oggFile);
  if(retcode)
    {
      std::cerr << "ov_open failed: ";
      ov_errcode(retcode);
      std::cerr << "\n";
      return false;
    }

  info = ov_info(&oggFile, -1);
  if(!info)
    {
      std::cerr << "loadVorbisFile: ov_info failed. \n";
      ov_clear(&oggFile);
      return false;
    }

  format = ( (1==info->channels)? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16 );
  freq = info->rate;

  do
    {
      bytes = ov_read(&oggFile, hunk, HUNKSIZE, endianness, 2, 1, &bitStream);
      if(bytes < 0)
	{
	  std::cerr << "ov_read failed: ";
	  ov_errcode(bytes);
	  std::cerr << std::endl;
	  ov_clear(&oggFile);
	  return false;
	}
      databuff.insert(databuff.end(), hunk, hunk + bytes);
    } while(bytes>0);

  alBufferData(AL_STATE.bufferName, format, &databuff[0], static_cast<ALsizei> (databuff.size()), freq);

  ov_clear(&oggFile);

  return true;
}

void ov_errcode(int ec)
{
  switch(ec)
    {
    case OV_EINVAL:
      std::cerr << "OV_EINVAL.";
      break;
    case OV_EBADLINK:
      std::cerr << "OV_EBADLINK.";
      break;
    case OV_EREAD:
      std::cerr << "OV_EREAD A read from media returned an error.";
      break;
    case OV_ENOTVORBIS:
      std::cerr << "OV_ENOTVORBIS Bitstream does not contain any Vorbis data.";
      break;
    case OV_EVERSION:
      std::cerr << "OV_EVERSION Vorbis version mismatch.";
      break;
    case OV_EBADHEADER:
      std::cerr << "OV_EBADHEADER Invalid Vorbis bitstream header.";
      break;
    case OV_EFAULT:
      std::cerr << "OV_EFAULT Internal logic fault; indicates a bug or heap/stack corruption.";
      break;
    case OV_HOLE:
      std::cerr << "OV_HOLE there was an interruption in the data.";
      break;
    default:
      std::cerr << "Unknown error code.";
      break;
    }
}
