/**
*	created:		2012-6-24   2:47
*	filename: 		Cpu
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include <windows.h>
//------------------------------------------------------------------------
static bool cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx)
{
	bool retval = true;
	unsigned long local_eax, local_ebx, local_ecx, local_edx;
	_asm pushad;

	__try
	{
        _asm
		{
			xor edx, edx		// Clue the compiler that EDX is about to be used.
            mov eax, function   // set up CPUID to return processor version and features
								//      0 = vendor string, 1 = version info, 2 = cache info
            cpuid				// code bytes = 0fh,  0a2h
            mov local_eax, eax	// features returned in eax
            mov local_ebx, ebx	// features returned in ebx
            mov local_ecx, ecx	// features returned in ecx
            mov local_edx, edx	// features returned in edx
		}
    } 
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{ 
		retval = false; 
	}

	out_eax = local_eax;
	out_ebx = local_ebx;
	out_ecx = local_ecx;
	out_edx = local_edx;

	_asm popad

	return retval;
}
//------------------------------------------------------------------------
bool CheckMMXTechnology(void)
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & 0x800000 ) != 0;
}
//------------------------------------------------------------------------
bool CheckSSETechnology(void)
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & 0x2000000L ) != 0;
}
//------------------------------------------------------------------------
bool CheckSSE2Technology(void)
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & 0x04000000 ) != 0;
}
//------------------------------------------------------------------------
bool Check3DNowTechnology(void)
{
    unsigned long eax, unused;
    if( !cpuid(0x80000000,eax,unused,unused,unused) )
		return false;

    if ( eax > 0x80000000L )
    {
     	if( !cpuid(0x80000001,unused,unused,unused,eax) )
			return false;

		return ( eax & 1<<31 ) != 0;
    }
    return false;
}
//------------------------------------------------------------------------
bool CheckCMOVTechnology()
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & (1<<15) ) != 0;
}
//------------------------------------------------------------------------
bool CheckFCMOVTechnology(void)
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & (1<<16) ) != 0;
}
//------------------------------------------------------------------------
bool CheckRDTSCTechnology(void)
{
    unsigned long eax,ebx,edx,unused;
    if( !cpuid(1,eax,ebx,unused,edx) )
		return false;

    return ( edx & 0x10 ) != 0;
}
//------------------------------------------------------------------------
// Return the Processor's vendor identification string, or "Generic_x86" if it doesn't exist on this CPU
const char* GetProcessorVendorId()
{
	unsigned long unused, VendorIDRegisters[3];

	static char VendorID[13];
	
	memset( VendorID, 0, sizeof(VendorID) );
	if( !cpuid(0,unused, VendorIDRegisters[0], VendorIDRegisters[2], VendorIDRegisters[1] ) )
	{
		strcpy( VendorID, "Generic_x86" ); 
	}
	else
	{
		memcpy( VendorID+0, &(VendorIDRegisters[0]), sizeof( VendorIDRegisters[0] ) );
		memcpy( VendorID+4, &(VendorIDRegisters[1]), sizeof( VendorIDRegisters[1] ) );
		memcpy( VendorID+8, &(VendorIDRegisters[2]), sizeof( VendorIDRegisters[2] ) );
	}


	return VendorID;
}
//------------------------------------------------------------------------
// Returns non-zero if Hyper-Threading Technology is supported on the processors and zero if not.  This does not mean that 
// Hyper-Threading Technology is necessarily enabled.
bool HTSupported(void)
{
	const unsigned int HT_BIT		 = 0x10000000;  // EDX[28] - Bit 28 set indicates Hyper-Threading Technology is supported in hardware.
	const unsigned int FAMILY_ID     = 0x0f00;      // EAX[11:8] - Bit 11 thru 8 contains family processor id
	const unsigned int EXT_FAMILY_ID = 0x0f00000;	// EAX[23:20] - Bit 23 thru 20 contains extended family  processor id
	const unsigned int PENTIUM4_ID   = 0x0f00;		// Pentium 4 family processor id

	unsigned long unused,
				  reg_eax = 0, 
				  reg_edx = 0,
				  vendor_id[3] = {0, 0, 0};

	// verify cpuid instruction is supported
	if( !cpuid(0,unused, vendor_id[0],vendor_id[2],vendor_id[1]) 
	 || !cpuid(1,reg_eax,unused,unused,reg_edx) )
	 return false;

	//  Check to see if this is a Pentium 4 or later processor
	if (((reg_eax & FAMILY_ID) ==  PENTIUM4_ID) || (reg_eax & EXT_FAMILY_ID))
		if (vendor_id[0] == 'uneG' && vendor_id[1] == 'Ieni' && vendor_id[2] == 'letn')
			return (reg_edx & HT_BIT) != 0;	// Genuine Intel Processor with Hyper-Threading Technology

	return false;  // This is not a genuine Intel processor.
}
//------------------------------------------------------------------------
bool SpeedstepSupported(void)
{
    unsigned long eax,ebx,edx,ecx;
    if( !cpuid(1,eax,ebx,ecx,edx) )
		return false;

    return ( ecx & 0x07 ) != 0;
}
//------------------------------------------------------------------------
// Returns the number of logical processors per physical processors.
static unsigned char LogicalProcessorsPerPackage(void)
{
	const unsigned NUM_LOGICAL_BITS = 0x00FF0000; // EBX[23:16] indicate number of logical processors per package

    unsigned long unused,
		          reg_ebx = 0;
    if (!HTSupported()) 
		return 1; 

	if( !cpuid(1,unused,reg_ebx,unused,unused) )
		return 1;

	return (unsigned char) ((reg_ebx & NUM_LOGICAL_BITS) >> 16);
}

//------------------------------------------------------------------------
#define X86_VENDOR_INTEL	0
#define X86_VENDOR_CYRIX	1
#define X86_VENDOR_AMD		2
#define X86_VENDOR_UMC		3
#define X86_VENDOR_NEXGEN	4
#define X86_VENDOR_CENTAUR	5
#define X86_VENDOR_RISE		6
#define X86_VENDOR_TRANSMETA 7
#define X86_VENDOR_UNKNOWN 0xff
#define MSR_IA32_PLATFORM_ID            0x17
#define MSR_IA32_EBL_CR_POWERON         0x2a
#define SPEEDSTEP_CHIPSET_ICH2M         0x00000002
#define SPEEDSTEP_CHIPSET_ICH3M         0x00000003
#define SPEEDSTEP_CHIPSET_ICH4M         0x00000004
#define SPEEDSTEP_PROCESSOR_PIII_C_EARLY	0x00000001  /* Coppermine core */
#define SPEEDSTEP_PROCESSOR_PIII_C		0x00000002  /* Coppermine core */
#define SPEEDSTEP_PROCESSOR_PIII_T 		0x00000003  /* Tualatin core */
#define SPEEDSTEP_PROCESSOR_P4M			0x00000004  /* P4-M with 100 MHz FSB */
//------------------------------------------------------------------------
struct cpuinfo_x86 {
        BYTE    x86;            /* CPU family */
        BYTE    x86_vendor;     /* CPU vendor */
        BYTE    x86_model;
        BYTE    x86_mask;
        DWORD   cpuid_level;    /* Maximum supported CPUID level, -1=no CPUID */
        DWORD   x86_capability[16];
        char    x86_vendor_id[16];
        char    x86_model_id[64];
        int     x86_cache_size;  /* in KB - valid for CPUS which support this
                                    call  */
        int     x86_clflush_size;
        int     x86_tlbsize;    /* number of 4K pages in DTLB/ITLB combined(in pages)*/
        unsigned long loops_per_jiffy;
}; 
//------------------------------------------------------------------------
void identify_cpu( cpuinfo_x86 * c )
{
		c->cpuid_level = -1;
        c->x86_vendor = X86_VENDOR_UNKNOWN;
        c->x86_model = c->x86_mask = 0; /* So far unknown... */
        memset( c->x86_vendor_id , 0 , sizeof(c->x86_vendor_id) ); /* Unset */
		memset( c->x86_model_id , 0 , sizeof(c->x86_model_id) );

       /* Get vendor name */
       DWORD & v1 = *((DWORD *)&c->x86_vendor_id[0]);
	   DWORD & v2 = *((DWORD *)&c->x86_vendor_id[8]);
	   DWORD & v3 = *((DWORD *)&c->x86_vendor_id[4]);
       cpuid(0x00000000, c->cpuid_level,v1,v2,v3);
                
        char *v = c->x86_vendor_id;
 
         if (!strcmp(v, "GenuineIntel"))
	               c->x86_vendor = X86_VENDOR_INTEL;
         else if (!strcmp(v, "AuthenticAMD"))
                 c->x86_vendor = X86_VENDOR_AMD;
         else if (!strcmp(v, "CyrixInstead"))
                 c->x86_vendor = X86_VENDOR_CYRIX;
         else if (!strcmp(v, "CentaurHauls"))
                 c->x86_vendor = X86_VENDOR_CENTAUR;
         else if (!strcmp(v, "NexGenDriven"))
                 c->x86_vendor = X86_VENDOR_NEXGEN;
         else if (!strcmp(v, "RiseRiseRise"))
                 c->x86_vendor = X86_VENDOR_RISE;
         else if (!strcmp(v, "GenuineTMx86") ||
                 !strcmp(v, "TransmetaCPU"))
                 c->x86_vendor = X86_VENDOR_TRANSMETA;
         else
                 c->x86_vendor = X86_VENDOR_UNKNOWN;

        /* Initialize the standard set of capabilities */
        /* Note that the vendor-specific code below might override */

        /* Intel-defined flags: level 0x00000001 */
        if ( c->cpuid_level >= 0x00000001 ) {   
                DWORD tfms,misc,junk;
				DWORD & capability = *((DWORD *)&c->x86_capability[0]);
                cpuid(0x00000001, tfms, misc, junk,capability);
                c->x86 = (tfms >> 8) & 15;
                if (c->x86 == 0xf)
                        c->x86 += (tfms >> 20) & 0xff;
                c->x86_model = (tfms >> 4) & 15;
                if (c->x86_model == 0xf)
                        c->x86_model += ((tfms >> 16) & 0xF) << 4; 
                 c->x86_mask = tfms & 15;
                if (c->x86_capability[0] & (1<<19)) 
                        c->x86_clflush_size = ((misc >> 8) & 0xff) * 8;
        } else {
                /* Have CPUID level 0 only - unheard of */
                c->x86 = 4;
        }
}
//------------------------------------------------------------------------
unsigned long cpuid_ebx( unsigned long op )
{
	unsigned long eax , ebx , ecx , edx ;
	cpuid( op , eax , ebx , ecx , edx );
	return ebx;
}
//------------------------------------------------------------------------
unsigned int speedstep_detect_processor (void)
{
	cpuinfo_x86 cc;
	cpuinfo_x86 *c = &cc;
	identify_cpu( c );
	DWORD ebx;

	if ((c->x86_vendor != X86_VENDOR_INTEL) || 
		((c->x86 != 6) && (c->x86 != 0xF)))
		return 0;

	if( c->x86 == 6 )
	{
		if (c->x86_model == 9 && c->x86_mask == 5 )
			return TRUE;

		if (c->x86_model == 13 && c->x86_mask == 6 )
			return TRUE;

		if (c->x86_model == 13 && c->x86_mask == 1 )
			return TRUE;
	}

	if (c->x86 == 0xF) {
		/* Intel Mobile Pentium 4-M
		* or Intel Mobile Pentium 4 with 533 MHz FSB */
		if (c->x86_model != 2)
			return 0;

		ebx = cpuid_ebx(0x00000001);
		ebx &= 0x000000FF;

		switch (c->x86_mask) {
		case 4: 
		/*
		* B-stepping [M-P4-M] 
		* sample has ebx = 0x0f, production has 0x0e.
		*/
		if ((ebx == 0x0e) || (ebx == 0x0f))
			return SPEEDSTEP_PROCESSOR_P4M;
		break;
		case 7: 
		/*
		* C-stepping [M-P4-M]
		* needs to have ebx=0x0e, else it's a celeron:
		* cf. 25130917.pdf / page 7, footnote 5 even
		* though 25072120.pdf / page 7 doesn't say
		* samples are only of B-stepping...
		*/
		if (ebx == 0x0e)
		return SPEEDSTEP_PROCESSOR_P4M;
		break;
		case 9:
		/*
		* D-stepping [M-P4-M or M-P4/533]
		*
				* this is totally strange: CPUID 0x0F29 is
		* used by M-P4-M, M-P4/533 and(!) Celeron CPUs.
		* The latter need to be sorted out as they don't
		* support speedstep.
		* Celerons with CPUID 0x0F29 may have either
		* ebx=0x8 or 0xf -- 25130917.pdf doesn't say anything
		* specific.
		* M-P4-Ms may have either ebx=0xe or 0xf [see above]
		* M-P4/533 have either ebx=0xe or 0xf. [25317607.pdf]
		* So, how to distinguish all those processors with
		* ebx=0xf? I don't know. Sort them out, and wait
		* for someone to complain.
		*/
		if (ebx == 0x0e)
		return SPEEDSTEP_PROCESSOR_P4M;
		break;
		default:
		break;
		}
		return 0;
	}

	switch (c->x86_model) {
		case 0x0B: /* Intel PIII [Tualatin] */
		/* cpuid_ebx(1) is 0x04 for desktop PIII, 
		0x06 for mobile PIII-M */
		ebx = cpuid_ebx(0x00000001);

		ebx &= 0x000000FF;

		if (ebx != 0x06)
		return 0;

		/* So far all PIII-M processors support SpeedStep. See
		* Intel's 24540640.pdf of June 2003 
		*/
		return SPEEDSTEP_PROCESSOR_PIII_T;

		case 0x08: /* Intel PIII [Coppermine] */
		return SPEEDSTEP_PROCESSOR_PIII_C;

		default:
		return 0;
		}
}
//------------------------------------------------------------------------