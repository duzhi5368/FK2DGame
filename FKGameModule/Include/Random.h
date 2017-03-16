/**
*	created:		2012-6-25   23:49
*	filename: 		Random
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------
#define GENIUS_NUMBER 0x19731211
//------------------------------------------------------------------------
class  CRandom
{
	enum { Number=0x1000 };
	unsigned long *m_Buffer;
	unsigned long m_Seed;
	unsigned m_Ptr;

public:
	CRandom()
	{
		int s;
		m_Buffer=new unsigned long[Number];
		m_Ptr=Number*2;
		__asm {
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}
		m_Seed=s;
	}
	~CRandom()
	{
		delete[] m_Buffer;
	}

	void Seed(unsigned long seed)	// 固定种子
	{
		m_Seed=seed^GENIUS_NUMBER;
		Batch();
	}
	unsigned long Seed()	// 随机种子
	{
		int s;
		__asm {
			_asm _emit 0x0f _asm _emit 0x31
				mov s,eax
		}
		Seed(s);
		return s^GENIUS_NUMBER;
	}
	unsigned long GetSeed() const
	{
		return m_Seed^GENIUS_NUMBER;
	}

	unsigned short GetWord() // 读一个 Word
	{
		if (m_Ptr>=Number*2) {
			Batch();
			m_Ptr=0;
		}
		return *((unsigned short *)m_Buffer+m_Ptr++);
	}
	unsigned long GetDWord() // 读一个 DWord
	{
		if (m_Ptr>=Number*2-1) {
			Batch();
			m_Ptr=0;
		}
		m_Ptr+=2;
		return *(unsigned long*)((unsigned short *)m_Buffer+m_Ptr-2);
	}
private:
	void Batch()
	{
		__asm {
			cld
				mov edi,[ecx]CRandom.m_Buffer
				mov eax,[ecx]CRandom.m_Seed
				mov ecx,0x1000		;//WRandom::Number
			mov esi,0xE7BD2160

				ALIGN 4
_loop_circle1:
			push ecx
				mov ecx,32
_loop_circle2:
			mov ebx,eax
				shl eax,1
				and ebx,esi;	//select the bit for xor
			mov edx,ebx
				bswap ebx
				xor bx,dx
				xor bh,bl;		// because P only judge one byte
			;// so must XOR to judge the p of whole word 
			jp _next;		//jp equals the xor 
			inc eax
_next:
			dec ecx
				jnz _loop_circle2
				pop ecx
				stosd
				dec ecx
				jnz _loop_circle1
				mov ecx,this
				mov [ecx]CRandom.m_Seed,eax
		}
	}
};
//------------------------------------------------------------------------