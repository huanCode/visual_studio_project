#pragma once
#ifndef _PARSEHLS_H
#define _PARSEHLS_H
#include "amcomdef.h"
#include "ToolList.h"

struct segment {
	MInt64 duration;
	MInt64 url_offset;
	MInt64 size;
	MPChar url;

	/* associated Media Initialization Section, treated as a segment */
	struct segment *init_section;
};







class ParseHls
{
private:
	class Playlist
	{
	public:
		Playlist()
		{
			iBandwidth = 0;
			iDuration = 0;
			iCurrentIndex = 0;
			strUrl = MNull;
			//iSegments = 0;
			//ts = MNull;
		}
		void AddSegment(segment* ts);

		MPChar	strUrl;
		MInt32	iBandwidth;
		MInt32	iDuration;
		MInt32	iCurrentIndex;
		ToolList<segment*> segmentList;
		//MInt32	iSegments;
		//struct segment *ts;
	};

public:
	ParseHls();
	static MBool hls_probe(MPChar p_buffer, MUInt32 p_size);
	MBool Parse();
	MBool ParseM3u8(MPChar strUrl,Playlist* playlist = MNull);

private:
	enum m3u8Level
	{
		TopLevel,
		SecondLevel
	};





	Playlist* createPlaylist(Playlist* playlist)
	{
		if (playlist == MNull)
		{
			playlist = new Playlist();
		}

		return playlist;
	}


private:
	static void ff_parse_key_val_cb(void* srcData, MPChar key, MInt32 keyLen, MPChar value);

private:

	ToolList<Playlist*>		m_playlistList;
	MBool					m_playListType;
};




#endif // !_PARSEHLS_H