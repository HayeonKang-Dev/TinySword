#pragma once
#include "CoreMinimal.h"




#pragma pack(push, 1)
struct TINYSWORD_API FMessageHeader
{
	// uint32 Type;
	// uint32 Size;
	short Type;
	unsigned short Size; 
	

	static constexpr int RequestLength = sizeof(unsigned short) + sizeof(short);

	FMessageHeader()
		: Type(0), Size(0)
	{
	}

	FMessageHeader(uint32 Type, int32 PayloadSize)
		: Type(Type), Size(PayloadSize)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FMessageHeader& Header)
	{
		// Ar << Header.Type;
		// Ar << Header.Size;
		unsigned short TotalSize = static_cast<unsigned short>(Header.Size + 4);
		Ar << TotalSize; 
		Ar << Header.Type;
		return Ar;
	}

	// friend FArchive& operator >>(FArchive& Ar, FMessageHeader& Header)
	// {
	// 	Ar >> Header.Size; 
	// 	Ar >> Header.Type;
	// 	return Ar;
	// }
};
#pragma pack(pop)



DECLARE_STATS_GROUP(TEXT("TCPSocketClient"), STATGROUP_TCPSocketClient, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("Send"), STAT_Send, STATGROUP_TCPSocketClient);
DECLARE_CYCLE_STAT(TEXT("Recv"), STAT_Recv, STATGROUP_TCPSocketClient);



class TINYSWORD_API FTCPSocketClientUtils
{
public:
	static bool Connect(FSocket* Socket, FString IPAddress, int PortNumber);

	static void DestroySocket(FSocket* Socket);

	static void PrintSocketError(FString& OutText);
};