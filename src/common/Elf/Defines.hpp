#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// https://gabi.xinuos.com/elf.pdf
namespace common::Elf::Defines {
  using Elf32_Addr    = std::uint32_t;  // Unsigned program address
  using Elf32_Off     = std::uint32_t;  // Unsigned file offset
  using Elf32_Half    = std::uint16_t;  // Unsigned medium integer
  using Elf32_Word    = std::uint32_t;  // Unsigned integer
  using Elf32_Sword   = std::int32_t;   // Signed integer
  using Elf32_Byte    = unsigned char;  // Unsigned small integer
  
  using Elf64_Addr    = std::uint64_t;  // Unsigned program address
  using Elf64_Off     = std::uint64_t;  // Unsigned file offset
  using Elf64_Half    = std::uint16_t;  // Unsigned medium integer
  using Elf64_Word    = std::uint32_t;  // Unsigned integer
  using Elf64_Sword   = std::int32_t;   // Signed integer
  using Elf64_Xword   = std::uint64_t;  // Unsigned long integer
  using Elf64_Sxword  = std::int64_t;   // Signed long integer
  using Elf64_Byte    = unsigned char;  // Unsigned small integer

  enum class IdentIndex : std::size_t {
    EI_MAG0       = 0,  // File identification
    EI_MAG1       = 1,  // File identification
    EI_MAG2       = 2,  // File identification
    EI_MAG3       = 3,  // File identification
    EI_CLASS      = 4,  // File class
    EI_DATA       = 5,  // Data encoding
    EI_VERSION    = 6,  // File version
    EI_OSABI      = 7,  // Operating system/ABI identification
    EI_ABIVERSION = 8,  // ABI version
    EI_PAD        = 9,  // Start of padding bytes
    EI_NIDENT     = 16, // Size of e_ident[]
  };

  enum class IdentMagicNumber : Elf32_Byte {
    ELFMAG0 = 0x7f, // e_ident[EI_MAG0]
    ELFMAG1 = 'E',  // e_ident[EI_MAG1]
    ELFMAG2 = 'L',  // e_ident[EI_MAG2]
    ELFMAG3 = 'F',  // e_ident[EI_MAG3]
  };
  
  enum class IdentClass : Elf32_Byte {
    ELFCLASSNONE  = 0,  // Invalid class 
    ELFCLASS32    = 1,  // 32-bit objects
    ELFCLASS64    = 2,  // 64-bit objects
  };

  enum class IdentDataEncoding : Elf32_Byte {
    ELFDATANONE = 0, // Invalid data encoding
    ELFDATA2LSB = 1, // Least-Significant Byte stored in byte 0
    ELFDATA2MSB = 2, // Most-Significant Byte stored in byte 0
  };

  enum class IdentOsAbi : Elf32_Byte {
    ELFOSABI_NONE     = 0,  // No extensions or unspecified
    ELFOSABI_HPUX     = 1,  // Hewlett-Packard HP-UX
    ELFOSABI_NETBSD   = 2,  // NetBSD
    ELFOSABI_GNU      = 3,  // GNU
    ELFOSABI_LINUX    = 3,  // Linux (historical—alias for ELFOSABI_GNU)
    ELFOSABI_SOLARIS  = 6,  // Sun Solaris
    ELFOSABI_AIX      = 7,  // AIX
    ELFOSABI_IRIX     = 8,  // IRIX
    ELFOSABI_FREEBSD  = 9,  // FreeBSD
    ELFOSABI_TRU64    = 10, // Compaq TRU64 UNIX
    ELFOSABI_MODESTO  = 11, // Novell Modesto
    ELFOSABI_OPENBSD  = 12, // Open BSD
    ELFOSABI_OPENVMS  = 13, // Open VMS
    ELFOSABI_NSK      = 14, // Hewlett-Packard Non-Stop Kernel
    ELFOSABI_AROS     = 15, // Amiga Research OS
    ELFOSABI_FENIXOS  = 16, // The FenixOS highly scalable multi-core OS
    ELFOSABI_CLOUDABI = 17, // Nuxi CloudABI
    ELFOSABI_OPENVOS  = 18, // Stratus Technologies OpenVOS
  };

  enum class ObjectFileType : Elf32_Half {
    ET_NONE   = 0,      // No file type
    ET_REL    = 1,      // Relocatable file
    ET_EXEC   = 2,      // Executable file
    ET_DYN    = 3,      // Shared object file
    ET_CORE   = 4,      // Core file
    ET_LOOS   = 0xfe00, // Operating system-specific
    ET_HIOS   = 0xfeff, // Operating system-specific
    ET_LOPROC = 0xff00, // Processor-specific
    ET_HIPROC = 0xffff, // Processor-specific
  };
  
  enum class Machine : Elf32_Half {
    EM_NONE            = 0,    // No machine
    EM_M32             = 1,    // AT&T WE 32100
    EM_SPARC           = 2,    // SPARC
    EM_386             = 3,    // Intel 80386
    EM_68K             = 4,    // Motorola 68000
    EM_88K             = 5,    // Motorola 88000
    EM_IAMCU           = 6,    // Intel MCU
    EM_860             = 7,    // Intel 80860
    EM_MIPS            = 8,    // MIPS I Architecture
    EM_S370            = 9,    // IBM System/370 Processor
    EM_MIPS_RS3_LE     = 10,   // MIPS RS3000 Little-endian
    EM_RESERVED_11     = 11,   // Reserved for future use
    EM_RESERVED_12     = 12,   // Reserved for future use
    EM_RESERVED_13     = 13,   // Reserved for future use
    EM_RESERVED_14     = 14,   // Reserved for future use
    EM_PARISC          = 15,   // Hewlett-Packard PA-RISC
    EM_RESERVED_16     = 16,   // Reserved for future use
    EM_VPP500          = 17,   // Fujitsu VPP500
    EM_SPARC32PLUS     = 18,   // Enhanced instruction set SPARC
    EM_960             = 19,   // Intel 80960
    EM_PPC             = 20,   // PowerPC
    EM_PPC64           = 21,   // 64-bit PowerPC
    EM_S390            = 22,   // IBM System/390 Processor
    EM_SPU             = 23,   // IBM SPU/SPC
    EM_RESERVED_24     = 24,   // Reserved for future use
    EM_RESERVED_25     = 25,   // Reserved for future use
    EM_RESERVED_26     = 26,   // Reserved for future use
    EM_RESERVED_27     = 27,   // Reserved for future use
    EM_RESERVED_28     = 28,   // Reserved for future use
    EM_RESERVED_29     = 29,   // Reserved for future use
    EM_RESERVED_30     = 30,   // Reserved for future use
    EM_RESERVED_31     = 31,   // Reserved for future use
    EM_RESERVED_32     = 32,   // Reserved for future use
    EM_RESERVED_33     = 33,   // Reserved for future use
    EM_RESERVED_34     = 34,   // Reserved for future use
    EM_RESERVED_35     = 35,   // Reserved for future use
    EM_V800            = 36,   // NEC V800
    EM_FR20            = 37,   // Fujitsu FR20
    EM_RH32            = 38,   // TRW RH-32
    EM_RCE             = 39,   // Motorola RCE
    EM_ARM             = 40,   // ARM 32-bit architecture (AARCH32)
    EM_ALPHA           = 41,   // Digital Alpha
    EM_SH              = 42,   // Hitachi SH
    EM_SPARCV9         = 43,   // SPARC Version 9
    EM_TRICORE         = 44,   // Siemens TriCore embedded processor
    EM_ARC             = 45,   // Argonaut RISC Core, Argonaut Technologies Inc.
    EM_H8_300          = 46,   // Hitachi H8/300
    EM_H8_300H         = 47,   // Hitachi H8/300H
    EM_H8S             = 48,   // Hitachi H8S
    EM_H8_500          = 49,   // Hitachi H8/500
    EM_IA_64           = 50,   // Intel IA-64 processor architecture
    EM_MIPS_X          = 51,   // Stanford MIPS-X
    EM_COLDFIRE        = 52,   // Motorola ColdFire
    EM_68HC12          = 53,   // Motorola M68HC12
    EM_MMA             = 54,   // Fujitsu MMA Multimedia Accelerator
    EM_PCP             = 55,   // Siemens PCP
    EM_NCPU            = 56,   // Sony nCPU embedded RISC processor
    EM_NDR1            = 57,   // Denso NDR1 microprocessor
    EM_STARCORE        = 58,   // Motorola Star*Core processor
    EM_ME16            = 59,   // Toyota ME16 processor
    EM_ST100           = 60,   // STMicroelectronics ST100 processor
    EM_TINYJ           = 61,   // Advanced Logic Corp. TinyJ embedded processor family
    EM_X86_64          = 62,   // AMD x86-64 architecture
    EM_PDSP            = 63,   // Sony DSP Processor
    EM_PDP10           = 64,   // Digital Equipment Corp. PDP-10
    EM_PDP11           = 65,   // Digital Equipment Corp. PDP-11
    EM_FX66            = 66,   // Siemens FX66 microcontroller
    EM_ST9PLUS         = 67,   // STMicroelectronics ST9+ 8/16 bit microcontroller
    EM_ST7             = 68,   // STMicroelectronics ST7 8-bit microcontroller
    EM_68HC16          = 69,   // Motorola MC68HC16 Microcontroller
    EM_68HC11          = 70,   // Motorola MC68HC11 Microcontroller
    EM_68HC08          = 71,   // Motorola MC68HC08 Microcontroller
    EM_68HC05          = 72,   // Motorola MC68HC05 Microcontroller
    EM_SVX             = 73,   // Silicon Graphics SVx
    EM_ST19            = 74,   // STMicroelectronics ST19 8-bit microcontroller
    EM_VAX             = 75,   // Digital VAX
    EM_CRIS            = 76,   // Axis Communications 32-bit embedded processor
    EM_JAVELIN         = 77,   // Infineon Technologies 32-bit embedded processor
    EM_FIREPATH        = 78,   // Element 14 64-bit DSP Processor
    EM_ZSP             = 79,   // LSI Logic 16-bit DSP Processor
    EM_MMIX            = 80,   // Donald Knuth's educational 64-bit processor
    EM_HUANY           = 81,   // Harvard University machine-independent object files
    EM_PRISM           = 82,   // SiTera Prism
    EM_AVR             = 83,   // Atmel AVR 8-bit microcontroller
    EM_FR30            = 84,   // Fujitsu FR30
    EM_D10V            = 85,   // Mitsubishi D10V
    EM_D30V            = 86,   // Mitsubishi D30V
    EM_V850            = 87,   // NEC v850
    EM_M32R            = 88,   // Mitsubishi M32R
    EM_MN10300         = 89,   // Matsushita MN10300
    EM_MN10200         = 90,   // Matsushita MN10200
    EM_PJ              = 91,   // picoJava
    EM_OPENRISC        = 92,   // OpenRISC 32-bit embedded processor
    EM_ARC_COMPACT     = 93,   // ARC International ARCompact processor (old spelling/synonym: EM_ARC_A5)
    EM_XTENSA          = 94,   // Tensilica Xtensa Architecture
    EM_VIDEOCORE       = 95,   // Alphamosaic VideoCore processor
    EM_TMM_GPP         = 96,   // Thompson Multimedia General Purpose Processor
    EM_NS32K           = 97,   // National Semiconductor 32000 series
    EM_TPC             = 98,   // Tenor Network TPC processor
    EM_SNP1K           = 99,   // Trebia SNP 1000 processor
    EM_ST200           = 100,  // STMicroelectronics (www.st.com) ST200 microcontroller
    EM_IP2K            = 101,  // Ubicom IP2xxx microcontroller family
    EM_MAX             = 102,  // MAX Processor
    EM_CR              = 103,  // National Semiconductor CompactRISC microprocessor
    EM_F2MC16          = 104,  // Fujitsu F2MC16
    EM_MSP430          = 105,  // Texas Instruments embedded microcontroller msp430
    EM_BLACKFIN        = 106,  // Analog Devices Blackfin (DSP) processor
    EM_SE_C33          = 107,  // S1C33 Family of Seiko Epson processors
    EM_SEP             = 108,  // Sharp embedded microprocessor
    EM_ARCA            = 109,  // Arca RISC Microprocessor
    EM_UNICORE         = 110,  // Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University
    EM_EXCESS          = 111,  // eXcess: 16/32/64-bit configurable embedded CPU
    EM_DXP             = 112,  // Icera Semiconductor Inc. Deep Execution Processor
    EM_ALTERA_NIOS2    = 113,  // Altera Nios II soft-core processor
    EM_CRX             = 114,  // National Semiconductor CompactRISC CRX microprocessor
    EM_XGATE           = 115,  // Motorola XGATE embedded processor
    EM_C166            = 116,  // Infineon C16x/XC16x processor
    EM_M16C            = 117,  // Renesas M16C series microprocessors
    EM_DSPIC30F        = 118,  // Microchip Technology dsPIC30F Digital Signal Controller
    EM_CE              = 119,  // Freescale Communication Engine RISC core
    EM_M32C            = 120,  // Renesas M32C series microprocessors
    EM_RESERVED_121    = 121,  // Reserved for future use
    EM_RESERVED_122    = 122,  // Reserved for future use
    EM_RESERVED_123    = 123,  // Reserved for future use
    EM_RESERVED_124    = 124,  // Reserved for future use
    EM_RESERVED_125    = 125,  // Reserved for future use
    EM_RESERVED_126    = 126,  // Reserved for future use
    EM_RESERVED_127    = 127,  // Reserved for future use
    EM_RESERVED_128    = 128,  // Reserved for future use
    EM_RESERVED_129    = 129,  // Reserved for future use
    EM_RESERVED_130    = 130,  // Reserved for future use
    EM_TSK3000         = 131,  // Altium TSK3000 core
    EM_RS08            = 132,  // Freescale RS08 embedded processor
    EM_SHARC           = 133,  // Analog Devices SHARC family of 32-bit DSP processors
    EM_ECOG2           = 134,  // Cyan Technology eCOG2 microprocessor
    EM_SCORE7          = 135,  // Sunplus S+core7 RISC processor
    EM_DSP24           = 136,  // New Japan Radio (NJR) 24-bit DSP Processor
    EM_VIDEOCORE3      = 137,  // Broadcom VideoCore III processor
    EM_LATTICEMICO32   = 138,  // RISC processor for Lattice FPGA architecture
    EM_SE_C17          = 139,  // Seiko Epson C17 family
    EM_TI_C6000        = 140,  // The Texas Instruments TMS320C6000 DSP family
    EM_TI_C2000        = 141,  // The Texas Instruments TMS320C2000 DSP family
    EM_TI_C5500        = 142,  // The Texas Instruments TMS320C55x DSP family
    EM_TI_ARP32        = 143,  // Texas Instruments Application Specific RISC Processor, 32bit fetch
    EM_TI_PRU          = 144,  // Texas Instruments Programmable Realtime Unit
    EM_RESERVED_145    = 145,  // Reserved for future use
    EM_RESERVED_146    = 146,  // Reserved for future use
    EM_RESERVED_147    = 147,  // Reserved for future use
    EM_RESERVED_148    = 148,  // Reserved for future use
    EM_RESERVED_149    = 149,  // Reserved for future use
    EM_RESERVED_150    = 150,  // Reserved for future use
    EM_RESERVED_151    = 151,  // Reserved for future use
    EM_RESERVED_152    = 152,  // Reserved for future use
    EM_RESERVED_153    = 153,  // Reserved for future use
    EM_RESERVED_154    = 154,  // Reserved for future use
    EM_RESERVED_155    = 155,  // Reserved for future use
    EM_RESERVED_156    = 156,  // Reserved for future use
    EM_RESERVED_157    = 157,  // Reserved for future use
    EM_RESERVED_158    = 158,  // Reserved for future use
    EM_RESERVED_159    = 159,  // Reserved for future use
    EM_MMDSP_PLUS      = 160,  // STMicroelectronics 64bit VLIW Data Signal Processor
    EM_CYPRESS_M8C     = 161,  // Cypress M8C microprocessor
    EM_R32C            = 162,  // Renesas R32C series microprocessors
    EM_TRIMEDIA        = 163,  // NXP Semiconductors TriMedia architecture family
    EM_QDSP6           = 164,  // QUALCOMM DSP6 Processor
    EM_8051            = 165,  // Intel 8051 and variants
    EM_STXP7X          = 166,  // STMicroelectronics STxP7x family of configurable and extensible RISC processors
    EM_NDS32           = 167,  // Andes Technology compact code size embedded RISC processor family
    EM_ECOG1           = 168,  // Cyan Technology eCOG1X family
    EM_ECOG1X          = 168,  // Cyan Technology eCOG1X family
    EM_MAXQ30          = 169,  // Dallas Semiconductor MAXQ30 Core Micro-controllers
    EM_XIMO16          = 170,  // New Japan Radio (NJR) 16-bit DSP Processor
    EM_MANIK           = 171,  // M2000 Reconfigurable RISC Microprocessor
    EM_CRAYNV2         = 172,  // Cray Inc. NV2 vector architecture
    EM_RX              = 173,  // Renesas RX family
    EM_METAG           = 174,  // Imagination Technologies META processor architecture
    EM_MCST_ELBRUS     = 175,  // MCST Elbrus general purpose hardware architecture
    EM_ECOG16          = 176,  // Cyan Technology eCOG16 family
    EM_CR16            = 177,  // National Semiconductor CompactRISC CR16 16-bit microprocessor
    EM_ETPU            = 178,  // Freescale Extended Time Processing Unit
    EM_SLE9X           = 179,  // Infineon Technologies SLE9X core
    EM_L10M            = 180,  // Intel L10M
    EM_K10M            = 181,  // Intel K10M
    EM_RESERVED_182    = 182,  // Reserved for future Intel use
    EM_AARCH64         = 183,  // ARM 64-bit architecture (AARCH64)
    EM_RESERVED_184    = 184,  // Reserved for future ARM use
    EM_AVR32           = 185,  // Atmel Corporation 32-bit microprocessor family
    EM_STM8            = 186,  // STMicroeletronics STM8 8-bit microcontroller
    EM_TILE64          = 187,  // Tilera TILE64 multicore architecture family
    EM_TILEPRO         = 188,  // Tilera TILEPro multicore architecture family
    EM_MICROBLAZE      = 189,  // Xilinx MicroBlaze 32-bit RISC soft processor core
    EM_CUDA            = 190,  // NVIDIA CUDA architecture
    EM_TILEGX          = 191,  // Tilera TILE-Gx multicore architecture family
    EM_CLOUDSHIELD     = 192,  // CloudShield architecture family
    EM_COREA_1ST       = 193,  // KIPO-KAIST Core-A 1st generation processor family
    EM_COREA_2ND       = 194,  // KIPO-KAIST Core-A 2nd generation processor family
    EM_ARC_COMPACT2    = 195,  // Synopsys ARCompact V2
    EM_OPEN8           = 196,  // Open8 8-bit RISC soft processor core
    EM_RL78            = 197,  // Renesas RL78 family
    EM_VIDEOCORE5      = 198,  // Broadcom VideoCore V processor
    EM_78KOR           = 199,  // Renesas 78KOR family
    EM_56800EX         = 200,  // Freescale 56800EX Digital Signal Controller (DSC)
    EM_BA1             = 201,  // Beyond BA1 CPU architecture
    EM_BA2             = 202,  // Beyond BA2 CPU architecture
    EM_XCORE           = 203,  // XMOS xCORE processor family
    EM_MCHP_PIC        = 204,  // Microchip 8-bit PIC(r) family
    EM_INTEL205        = 205,  // Reserved by Intel
    EM_INTEL206        = 206,  // Reserved by Intel
    EM_INTEL207        = 207,  // Reserved by Intel
    EM_INTEL208        = 208,  // Reserved by Intel
    EM_INTEL209        = 209,  // Reserved by Intel
    EM_KM32            = 210,  // KM211 KM32 32-bit processor
    EM_KMX32           = 211,  // KM211 KMX32 32-bit processor
    EM_KMX16           = 212,  // KM211 KMX16 16-bit processor
    EM_KMX8            = 213,  // KM211 KMX8 8-bit processor
    EM_KVARC           = 214,  // KM211 KVARC processor
    EM_CDP             = 215,  // Paneve CDP architecture family
    EM_COGE            = 216,  // Cognitive Smart Memory Processor
    EM_COOL            = 217,  // Bluechip Systems CoolEngine
    EM_NORC            = 218,  // Nanoradio Optimized RISC
    EM_CSR_KALIMBA     = 219,  // CSR Kalimba architecture family
    EM_Z80             = 220,  // Zilog Z80
    EM_VISIUM          = 221,  // Controls and Data Services VISIUMcore processor
    EM_FT32            = 222,  // FTDI Chip FT32 high performance 32-bit RISC architecture
    EM_MOXIE           = 223,  // Moxie processor family
    EM_AMDGPU          = 224,  // AMD GPU architecture
    EM_RESERVED_225    = 225,  // Reserved for future use
    EM_RESERVED_226    = 226,  // Reserved for future use
    EM_RESERVED_227    = 227,  // Reserved for future use
    EM_RESERVED_228    = 228,  // Reserved for future use
    EM_RESERVED_229    = 229,  // Reserved for future use
    EM_RESERVED_230    = 230,  // Reserved for future use
    EM_RESERVED_231    = 231,  // Reserved for future use
    EM_RESERVED_232    = 232,  // Reserved for future use
    EM_RESERVED_233    = 233,  // Reserved for future use
    EM_RESERVED_234    = 234,  // Reserved for future use
    EM_RESERVED_235    = 235,  // Reserved for future use
    EM_RESERVED_236    = 236,  // Reserved for future use
    EM_RESERVED_237    = 237,  // Reserved for future use
    EM_RESERVED_238    = 238,  // Reserved for future use
    EM_RESERVED_239    = 239,  // Reserved for future use
    EM_RESERVED_240    = 240,  // Reserved for future use
    EM_RESERVED_241    = 241,  // Reserved for future use
    EM_RESERVED_242    = 242,  // Reserved for future use
    EM_RISCV           = 243,  // RISC-V
    EM_LANAI           = 244,  // Lanai processor
    EM_CEVA            = 245,  // CEVA Processor Architecture Family
    EM_CEVA_X2         = 246,  // CEVA X2 Processor Family
    EM_BPF             = 247,  // Linux BPF - in-kernel virtual machine
    EM_GRAPHCORE_IPU   = 248,  // Graphcore Intelligent Processing Unit
    EM_IMG1            = 249,  // Imagination Technologies
    EM_NFP             = 250,  // Netronome Flow Processor (NFP)
    EM_VE              = 251,  // NEC Vector Engine
    EM_CSKY            = 252,  // C-SKY processor family
    EM_ARC_COMPACT3_64 = 253,  // Synopsys ARCv2.3 64-bit
    EM_MCS6502         = 254,  // MOS Technology MCS 6502 processor
    EM_ARC_COMPACT3    = 255,  // Synopsys ARCv2.3 32-bit
    EM_KVX             = 256,  // Kalray VLIW core of the MPPA processor family
    EM_65816           = 257,  // WDC 65816/65C816
    EM_LOONGARCH       = 258,  // Loongson Loongarch
    EM_KF32            = 259,  // ChipON KungFu32
    EM_U16_U8CORE      = 260,  // LAPIS nX-U16/U8
    EM_TACHYUM         = 261,  // Reserved for Tachyum processor
    EM_56800EF         = 262,  // NXP 56800EF Digital Signal Controller (DSC)
    EM_SBF             = 263,  // Solana Bytecode Format
    EM_AIENGINE        = 264,  // AMD/Xilinx AIEngine architecture
    EM_SIMA_MLA        = 265,  // SiMa MLA
    EM_BANG            = 266,  // Cambricon BANG
    EM_LOONGGPU        = 267,  // Loongson LoongGPU
    EM_SW64            = 268,  // Wuxi Institute of Advanced Technology SW64
    EM_AIECTRLCODE     = 269,  // AMD/Xilinx AIEngine ctrlcode
  };
  
  enum class VersionNumber : Elf32_Word {
    EV_NONE    = 0, // Invalid version
    EV_CURRENT = 1, // Current version
  };
  
  enum class SpecialSectionIndexe : Elf32_Half {
    SHN_UNDEF     = 0,
    SHN_LORESERVE = 0xff00,
    SHN_LOPROC    = 0xff00,
    SHN_HIPROC    = 0xff1f,
    SHN_LOOS      = 0xff20,
    SHN_HIOS      = 0xff3f,
    SHN_ABS       = 0xfff1,
    SHN_COMMON    = 0xfff2,
    SHN_XINDEX    = 0xffff,
    SHN_HIRESERVE = 0xffff,
  };
  
  enum class SectionType : Elf32_Word {
    SHT_NULL          = 0,
    SHT_PROGBITS      = 1,
    SHT_SYMTAB        = 2,
    SHT_STRTAB        = 3,
    SHT_RELA          = 4,
    SHT_HASH          = 5,
    SHT_DYNAMIC       = 6,
    SHT_NOTE          = 7,
    SHT_NOBITS        = 8,
    SHT_REL           = 9,
    SHT_SHLIB         = 10,
    SHT_DYNSYM        = 11,
    SHT_INIT_ARRAY    = 14,
    SHT_FINI_ARRAY    = 15,
    SHT_PREINIT_ARRAY = 16,
    SHT_GROUP         = 17,
    SHT_SYMTAB_SHNDX  = 18,
    SHT_RELR          = 19,
    SHT_LOOS          = 0x60000000,
    SHT_HIOS          = 0x6fffffff,
    SHT_LOPROC        = 0x70000000,
    SHT_HIPROC        = 0x7fffffff,
    SHT_LOUSER        = 0x80000000,
    SHT_HIUSER        = 0xffffffff,
  };

  enum class SectionAttributeFlags32 : Elf32_Word {
    SHF_WRITE            = 0x1,
    SHF_ALLOC            = 0x2,
    SHF_EXECINSTR        = 0x4,
    SHF_MERGE            = 0x10,
    SHF_STRINGS          = 0x20,
    SHF_INFO_LINK        = 0x40,
    SHF_LINK_ORDER       = 0x80,
    SHF_OS_NONCONFORMING = 0x100,
    SHF_GROUP            = 0x200,
    SHF_TLS              = 0x400,
    SHF_COMPRESSED       = 0x800,
    SHF_MASKOS           = 0x0ff00000,
    SHF_MASKPROC         = 0xf0000000,
  };

  [[nodiscard]] constexpr SectionAttributeFlags32 operator|(SectionAttributeFlags32 lhs, SectionAttributeFlags32 rhs) noexcept {
    return static_cast<SectionAttributeFlags32>(std::to_underlying(lhs) | std::to_underlying(rhs));
  }

  [[nodiscard]] constexpr SectionAttributeFlags32 operator&(SectionAttributeFlags32 lhs, SectionAttributeFlags32 rhs) noexcept {
    return static_cast<SectionAttributeFlags32>(std::to_underlying(lhs) & std::to_underlying(rhs));
  }

  enum class SectionAttributeFlags64 : Elf64_Xword {
    SHF_WRITE            = 0x1,
    SHF_ALLOC            = 0x2,
    SHF_EXECINSTR        = 0x4,
    SHF_MERGE            = 0x10,
    SHF_STRINGS          = 0x20,
    SHF_INFO_LINK        = 0x40,
    SHF_LINK_ORDER       = 0x80,
    SHF_OS_NONCONFORMING = 0x100,
    SHF_GROUP            = 0x200,
    SHF_TLS              = 0x400,
    SHF_COMPRESSED       = 0x800,
    SHF_MASKOS           = 0x0ff00000,
    SHF_MASKPROC         = 0xf0000000,
  };

  [[nodiscard]] constexpr SectionAttributeFlags64 operator|(SectionAttributeFlags64 lhs, SectionAttributeFlags64 rhs) noexcept {
    return static_cast<SectionAttributeFlags64>(std::to_underlying(lhs) | std::to_underlying(rhs));
  }

  [[nodiscard]] constexpr SectionAttributeFlags64 operator&(SectionAttributeFlags64 lhs, SectionAttributeFlags64 rhs) noexcept {
    return static_cast<SectionAttributeFlags64>(std::to_underlying(lhs) & std::to_underlying(rhs));
  }

  enum class CompressionType : Elf32_Word {
    ELFCOMPRESS_ZLIB   = 1,
    ELFCOMPRESS_ZSTD   = 2,
    ELFCOMPRESS_LOOS   = 0x60000000,
    ELFCOMPRESS_HIOS   = 0x6fffffff,
    ELFCOMPRESS_LOPROC = 0x70000000,
    ELFCOMPRESS_HIPROC = 0x7fffffff,
  };

  enum class SectionGroupFlags : Elf32_Word {
    GRP_COMDAT   = 0x1,
    GRP_MASKOS   = 0x0ff00000,
    GRP_MASKPROC = 0xf0000000,
  };
  
  [[nodiscard]] constexpr SectionGroupFlags operator|(SectionGroupFlags lhs, SectionGroupFlags rhs) noexcept {
    return static_cast<SectionGroupFlags>(std::to_underlying(lhs) | std::to_underlying(rhs));
  }
    
  [[nodiscard]] constexpr SectionGroupFlags operator&(SectionGroupFlags lhs, SectionGroupFlags rhs) noexcept {
    return static_cast<SectionGroupFlags>(std::to_underlying(lhs) & std::to_underlying(rhs));
  }

  enum class SpecialSymbolTableIndex : Elf32_Word {
    STN_UNDEF = 0,
  };

  enum class SymbolBinding : Elf32_Byte {
    STB_LOCAL  = 0,
    STB_GLOBAL = 1,
    STB_WEAK   = 2,
    STB_LOOS   = 10,
    STB_HIOS   = 12,
    STB_LOPROC = 13,
    STB_HIPROC = 15,
  };

  enum class SymbolType : Elf32_Byte {
    STT_NOTYPE  = 0,
    STT_OBJECT  = 1,
    STT_FUNC    = 2,
    STT_SECTION = 3,
    STT_FILE    = 4,
    STT_COMMON  = 5,
    STT_TLS     = 6,
    STT_LOOS    = 10,
    STT_HIOS    = 12,
    STT_LOPROC  = 13,
    STT_HIPROC  = 15,
  };

  enum class SymbolVisibility : Elf32_Byte {
    STV_DEFAULT   = 0,
    STV_INTERNAL  = 1,
    STV_HIDDEN    = 2,
    STV_PROTECTED = 3,
    STV_EXPORTED  = 4,
    STV_SINGLETON = 5,
    STV_ELIMINATE = 6,
  };

  enum class SegmentType : Elf32_Word {
    PT_NULL    = 0,
    PT_LOAD    = 1,
    PT_DYNAMIC = 2,
    PT_INTERP  = 3,
    PT_NOTE    = 4,
    PT_SHLIB   = 5,
    PT_PHDR    = 6,
    PT_TLS     = 7,
    PT_LOOS    = 0x60000000,
    PT_HIOS    = 0x6fffffff,
    PT_LOPROC  = 0x70000000,
    PT_HIPROC  = 0x7fffffff,
  };

  enum class SegmentFlags : Elf32_Word {
    PF_X        = 0x1,        // Execute
    PF_W        = 0x2,        // Write
    PF_R        = 0x4,        // Read
    PF_MASKOS   = 0x0ff00000, // Unspecified
    PF_MASKPROC = 0xf0000000, // Unspecified
  };
  
  [[nodiscard]] constexpr SegmentFlags operator|(SegmentFlags lhs, SegmentFlags rhs) noexcept {
    return static_cast<SegmentFlags>(std::to_underlying(lhs) | std::to_underlying(rhs));
  }
  
  [[nodiscard]] constexpr SegmentFlags operator&(SegmentFlags lhs, SegmentFlags rhs) noexcept {
    return static_cast<SegmentFlags>(std::to_underlying(lhs) & std::to_underlying(rhs));
  }

  enum class DynamicArrayTag32 : Elf32_Sword {
    DT_NULL             = 0,
    DT_NEEDED           = 1,
    DT_PLTRELSZ         = 2,
    DT_PLTGOT           = 3,
    DT_HASH             = 4,
    DT_STRTAB           = 5,
    DT_SYMTAB           = 6,
    DT_RELA             = 7,
    DT_RELASZ           = 8,
    DT_RELAENT          = 9,
    DT_STRSZ            = 10,
    DT_SYMENT           = 11,
    DT_INIT             = 12,
    DT_FINI             = 13,
    DT_SONAME           = 14,
    DT_RPATH            = 15,
    DT_SYMBOLIC         = 16,
    DT_REL              = 17,
    DT_RELSZ            = 18,
    DT_RELENT           = 19,
    DT_PLTREL           = 20,
    DT_DEBUG            = 21,
    DT_TEXTREL          = 22,
    DT_JMPREL           = 23,
    DT_BIND_NOW         = 24,
    DT_INIT_ARRAY       = 25,
    DT_FINI_ARRAY       = 26,
    DT_INIT_ARRAYSZ     = 27,
    DT_FINI_ARRAYSZ     = 28,
    DT_RUNPATH          = 29,
    DT_FLAGS            = 30,
    DT_ENCODING         = 32,
    DT_PREINIT_ARRAY    = 32,
    DT_PREINIT_ARRAYSZ  = 33,
    DT_SYMTAB_SHNDX     = 34,
    DT_RELRSZ           = 35,
    DT_RELR             = 36,
    DT_RELRENT          = 37,
    DT_SYMTABSZ         = 39,
    DT_LOOS             = 0x6000000D,
    DT_HIOS             = 0x6ffff000,
    DT_LOPROC           = 0x70000000,
    DT_HIPROC           = 0x7fffffff,
  };

  enum class DynamicArrayTag64 : Elf64_Sxword {
    DT_NULL             = 0,
    DT_NEEDED           = 1,
    DT_PLTRELSZ         = 2,
    DT_PLTGOT           = 3,
    DT_HASH             = 4,
    DT_STRTAB           = 5,
    DT_SYMTAB           = 6,
    DT_RELA             = 7,
    DT_RELASZ           = 8,
    DT_RELAENT          = 9,
    DT_STRSZ            = 10,
    DT_SYMENT           = 11,
    DT_INIT             = 12,
    DT_FINI             = 13,
    DT_SONAME           = 14,
    DT_RPATH            = 15,
    DT_SYMBOLIC         = 16,
    DT_REL              = 17,
    DT_RELSZ            = 18,
    DT_RELENT           = 19,
    DT_PLTREL           = 20,
    DT_DEBUG            = 21,
    DT_TEXTREL          = 22,
    DT_JMPREL           = 23,
    DT_BIND_NOW         = 24,
    DT_INIT_ARRAY       = 25,
    DT_FINI_ARRAY       = 26,
    DT_INIT_ARRAYSZ     = 27,
    DT_FINI_ARRAYSZ     = 28,
    DT_RUNPATH          = 29,
    DT_FLAGS            = 30,
    DT_ENCODING         = 32,
    DT_PREINIT_ARRAY    = 32,
    DT_PREINIT_ARRAYSZ  = 33,
    DT_SYMTAB_SHNDX     = 34,
    DT_RELRSZ           = 35,
    DT_RELR             = 36,
    DT_RELRENT          = 37,
    DT_SYMTABSZ         = 39,
    DT_LOOS             = 0x6000000D,
    DT_HIOS             = 0x6ffff000,
    DT_LOPROC           = 0x70000000,
    DT_HIPROC           = 0x7fffffff,
  };

  enum class DynamicArrayTagFlags : Elf32_Word {
    DF_ORIGIN     = 0x1,
    DF_SYMBOLIC   = 0x2,
    DF_TEXTREL    = 0x4,
    DF_BIND_NOW   = 0x8,
    DF_STATIC_TLS = 0x10,
  };

  [[nodiscard]] constexpr DynamicArrayTagFlags operator|(DynamicArrayTagFlags lhs, DynamicArrayTagFlags rhs) noexcept {
    return static_cast<DynamicArrayTagFlags>(std::to_underlying(lhs) | std::to_underlying(rhs));
  }

  [[nodiscard]] constexpr DynamicArrayTagFlags operator&(DynamicArrayTagFlags lhs, DynamicArrayTagFlags rhs) noexcept {
    return static_cast<DynamicArrayTagFlags>(std::to_underlying(lhs) & std::to_underlying(rhs));
  }

  struct Elf32_Ehdr {
    Elf32_Byte              e_ident[std::to_underlying(IdentIndex::EI_NIDENT)]{};
    ObjectFileType          e_type{};
    Machine                 e_machine{};
    VersionNumber           e_version{};
    Elf32_Addr              e_entry{};
    Elf32_Off               e_phoff{};
    Elf32_Off               e_shoff{};
    Elf32_Word              e_flags{};
    Elf32_Half              e_ehsize{};
    Elf32_Half              e_phentsize{};
    Elf32_Half              e_phnum{};
    Elf32_Half              e_shentsize{};
    Elf32_Half              e_shnum{};
    Elf32_Half              e_shstrndx{};
  };

  struct Elf64_Ehdr {
    Elf32_Byte              e_ident[std::to_underlying(IdentIndex::EI_NIDENT)]{};
    ObjectFileType          e_type{};
    Machine                 e_machine{};
    VersionNumber           e_version{};
    Elf64_Addr              e_entry{};
    Elf64_Off               e_phoff{};
    Elf64_Off               e_shoff{};
    Elf64_Word              e_flags{};
    Elf64_Half              e_ehsize{};
    Elf64_Half              e_phentsize{};
    Elf64_Half              e_phnum{};
    Elf64_Half              e_shentsize{};
    Elf64_Half              e_shnum{};
    Elf64_Half              e_shstrndx{};
  };

  struct Elf32_Shdr {
    Elf32_Word              sh_name{};
    SectionType             sh_type{};
    SectionAttributeFlags32 sh_flags{};
    Elf32_Addr              sh_addr{};
    Elf32_Off               sh_offset{};
    Elf32_Word              sh_size{};
    Elf32_Word              sh_link{};
    Elf32_Word              sh_info{};
    Elf32_Word              sh_addralign{};
    Elf32_Word              sh_entsize{};
  };

  struct Elf64_Shdr {
    Elf64_Word              sh_name{};
    SectionType             sh_type{};
    SectionAttributeFlags64 sh_flags{};
    Elf64_Addr              sh_addr{};
    Elf64_Off               sh_offset{};
    Elf64_Xword             sh_size{};
    Elf64_Word              sh_link{};
    Elf64_Word              sh_info{};
    Elf64_Xword             sh_addralign{};
    Elf64_Xword             sh_entsize{};
  };

  struct Elf32_Chdr {
    CompressionType ch_type{};
    Elf32_Word      ch_size{};
    Elf32_Word      ch_addralign{};
  };

  struct Elf64_Chdr {
    CompressionType ch_type{};
    Elf64_Word      ch_reserved{};
    Elf64_Xword     ch_size{};
    Elf64_Xword     ch_addralign{};
  };

  struct Elf32_Sym {
    Elf32_Word st_name{};
    Elf32_Addr st_value{};
    Elf32_Word st_size{};
    Elf32_Byte st_info{};
    Elf32_Byte st_other{};
    Elf32_Half st_shndx{};
  };

  struct Elf64_Sym {
    Elf64_Word  st_name{};
    Elf64_Byte  st_info{};
    Elf64_Byte  st_other{};
    Elf64_Half  st_shndx{};
    Elf64_Addr  st_value{};
    Elf64_Xword st_size{};
  };

  [[nodiscard]] constexpr SymbolBinding ELF32_ST_BIND(Elf32_Byte st_info) noexcept {
    return static_cast<SymbolBinding>(st_info >> 4);
  }

  [[nodiscard]] constexpr SymbolType ELF32_ST_TYPE(Elf32_Byte st_info) noexcept {
    return static_cast<SymbolType>(st_info & 0xf);
  }

  [[nodiscard]] constexpr Elf32_Byte ELF32_ST_INFO(SymbolBinding bind, SymbolType type) noexcept {
    return static_cast<Elf32_Byte>((std::to_underlying(bind) << 4) + (std::to_underlying(type) & 0xf));
  }

  [[nodiscard]] constexpr SymbolBinding ELF64_ST_BIND(Elf64_Byte st_info) noexcept {
    return static_cast<SymbolBinding>(st_info >> 4);
  }

  [[nodiscard]] constexpr SymbolType ELF64_ST_TYPE(Elf64_Byte st_info) noexcept {
    return static_cast<SymbolType>(st_info & 0xf);
  }

  [[nodiscard]] constexpr Elf64_Byte ELF64_ST_INFO(SymbolBinding bind, SymbolType type) noexcept {
    return static_cast<Elf64_Byte>((std::to_underlying(bind) << 4) + (std::to_underlying(type) & 0xf));
  }

  [[nodiscard]] constexpr SymbolVisibility ELF32_ST_VISIBILITY(Elf32_Byte st_other) noexcept {
    return static_cast<SymbolVisibility>(st_other & 0x7);
  }

  [[nodiscard]] constexpr SymbolVisibility ELF64_ST_VISIBILITY(Elf64_Byte st_other) noexcept {
    return static_cast<SymbolVisibility>(st_other & 0x7);
  }

  struct Elf32_Rel {
    Elf32_Addr r_offset{};
    Elf32_Word r_info{};
  };

  struct Elf32_Rela {
    Elf32_Addr  r_offset{};
    Elf32_Word  r_info{};
    Elf32_Sword r_addend{};
  };

  struct Elf64_Rel {
    Elf64_Addr  r_offset{};
    Elf64_Xword r_info{};
  };

  struct Elf64_Rela {
    Elf64_Addr    r_offset{};
    Elf64_Xword   r_info{};
    Elf64_Sxword  r_addend{};
  };

  [[nodiscard]] constexpr Elf32_Word ELF32_R_SYM(Elf32_Word r_info) noexcept {
    return r_info >> 8;
  }

  [[nodiscard]] constexpr Elf32_Byte ELF32_R_TYPE(Elf32_Word r_info) noexcept {
    return r_info & 0xff;
  }
  
  [[nodiscard]] constexpr Elf32_Word ELF32_R_INFO(Elf32_Word sym, Elf32_Byte type) noexcept {
    return (sym << 8) + type;
  }

  [[nodiscard]] constexpr Elf64_Word ELF64_R_SYM(Elf64_Xword r_info) noexcept {
    return r_info >> 32;
  }
  
  [[nodiscard]] constexpr Elf64_Word ELF64_R_TYPE(Elf64_Xword r_info) noexcept {
    return r_info & 0xffffffffL;
  }
  
  [[nodiscard]] constexpr Elf64_Xword ELF64_R_INFO(Elf64_Word sym, Elf64_Word type) noexcept {
    return (static_cast<Elf64_Xword>(sym) << 32) + (type & 0xffffffffL);
  }

  using Elf32_Relr = Elf32_Word;
  using Elf64_Relr = Elf64_Xword;

  struct Elf32_Phdr {
    SegmentType   p_type{};
    Elf32_Off     p_offset{};
    Elf32_Addr    p_vaddr{};
    Elf32_Addr    p_paddr{};
    Elf32_Word    p_filesz{};
    Elf32_Word    p_memsz{};
    SegmentFlags  p_flags{};
    Elf32_Word    p_align{};
  };

  struct Elf64_Phdr {
    SegmentType   p_type{};
    SegmentFlags  p_flags{};
    Elf64_Off     p_offset{};
    Elf64_Addr    p_vaddr{};
    Elf64_Addr    p_paddr{};
    Elf64_Xword   p_filesz{};
    Elf64_Xword   p_memsz{};
    Elf64_Xword   p_align{};
  };

  struct Elf32_Dyn {
    DynamicArrayTag32 d_tag{};
    union {
      Elf32_Word d_val;
      Elf32_Addr d_ptr;
    } d_un{};
  };
  
  // extern Elf32_Dyn _DYNAMIC[];
  
  struct Elf64_Dyn {
    DynamicArrayTag64 d_tag{};
    union {
      Elf64_Xword d_val;
      Elf64_Addr d_ptr;
    } d_un{};
  };

  // extern Elf64_Dyn _DYNAMIC[];

  [[nodiscard]] constexpr unsigned long elf_hash(const unsigned char* name) noexcept {
    unsigned long h = 0, g;
    while (*name) {
      h = (h << 4) + *name++;
      if (g = h & 0xf0000000; g)
        h ^= g >> 24;
      h &= ~g;
    }
    return h;
  }

  static_assert(sizeof(Elf32_Ehdr) == 52);
  static_assert(std::is_trivially_copyable_v<Elf32_Ehdr>);
  static_assert(sizeof(Elf64_Ehdr) == 64);
  static_assert(std::is_trivially_copyable_v<Elf64_Ehdr>);

  static_assert(sizeof(Elf32_Shdr) == 40);
  static_assert(std::is_trivially_copyable_v<Elf32_Shdr>);
  static_assert(sizeof(Elf64_Shdr) == 64);
  static_assert(std::is_trivially_copyable_v<Elf64_Shdr>);

  static_assert(sizeof(Elf32_Phdr) == 32);
  static_assert(std::is_trivially_copyable_v<Elf32_Phdr>);
  static_assert(sizeof(Elf64_Phdr) == 56);
  static_assert(std::is_trivially_copyable_v<Elf64_Phdr>);

  static_assert(sizeof(Elf32_Sym) == 16);
  static_assert(std::is_trivially_copyable_v<Elf32_Sym>);
  static_assert(sizeof(Elf64_Sym) == 24);
  static_assert(std::is_trivially_copyable_v<Elf64_Sym>);

  static_assert(sizeof(Elf32_Rel) == 8);
  static_assert(std::is_trivially_copyable_v<Elf32_Rel>);
  static_assert(sizeof(Elf64_Rel) == 16);
  static_assert(std::is_trivially_copyable_v<Elf64_Rel>);

  static_assert(sizeof(Elf32_Rela) == 12);
  static_assert(std::is_trivially_copyable_v<Elf32_Rela>);
  static_assert(sizeof(Elf64_Rela) == 24);
  static_assert(std::is_trivially_copyable_v<Elf64_Rela>);
}
