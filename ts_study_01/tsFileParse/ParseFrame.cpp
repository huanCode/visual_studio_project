#include "stdafx.h"
#include "ParseFrame.h"
#include "ParseHls.h"

ParseFrame::ParseFrame()
{
	//m_probeList.AddNode(ParseHls::hls_probe);

}

MBool ParseFrame::FindParse(MPChar pbuf, MInt32 iBufSize)
{
	read_probe probeFuntion = MNull;
	//for (MInt32 i = 0; i < m_probeList.GetSize(); i++)
	//{
	//	probeFuntion = m_probeList.GetNodePtrByIndex(i);
	//	probeFuntion(pbuf, iBufSize);
	//}

	return ParseHls::hls_probe(pbuf, iBufSize);


}