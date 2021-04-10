/*
   Register access classes for RISC-V system registers.
   SPDX-License-Identifier: Unlicense

   https://five-embeddev.com/

*/

#ifndef RISCV_CSR_HPP
#define RISCV_CSR_HPP

#include <cstdint>

// ------------------------------------------------------------------------
// Base and common classes

namespace riscv {
    namespace csr {
#if __riscv_xlen==32 
        using uint_xlen_t  = std::uint32_t;
        using uint_csr32_t = std::uint32_t;
        using uint_csr64_t = std::uint32_t;
#elif __riscv_xlen==64
        using uint_xlen_t  = std::uint64_t;
        using uint_csr32_t = std::uint32_t;
        using uint_csr64_t = std::uint64_t;
#else
        using uint_xlen_t  = std::uint32_t;
        using uint_csr32_t = std::uint32_t;
        using uint_csr64_t = std::uint32_t;
#error "riscv::csr: unknown __riscv_xlen"
#endif
        /** Immediate instructions use a 5 bit immediate field */
        static constexpr uint_xlen_t CSR_IMM_OP_MASK = 0x01F; 
        
        /** CSR: Read only, and read-write base class */
        template<class C> class read_only_reg {
        public :
            using read_datatype_t = typename C::datatype;

            read_only_reg(void) {}
            read_only_reg(const read_only_reg&) = delete;
            read_only_reg& operator=(const read_only_reg&) = delete;
        
            /** Read the CSR value */
            static inline read_datatype_t read(void) {
                return C::read();
            }
            /** Operator alias to read the CSR value */
            inline read_datatype_t operator()(void) {
                return C::read();
            }
        };
        /** CSR: Write only, and read-write base class */
        template<class C> class write_only_reg {
        public :
            using write_datatype_t = typename C::datatype;

            write_only_reg(void) {}
            write_only_reg(const write_only_reg&) = delete;
            write_only_reg& operator=(const write_only_reg&) = delete;

            /** Write a constant to the CSR. */
            template<write_datatype_t VALUE> void write_const(void) {
                if constexpr ((VALUE & CSR_IMM_OP_MASK) == VALUE) {
                    C::write_imm(VALUE);
                } else {
                    C::write(VALUE);
                }
            }
            /** Write to the CSR. */
            inline void write(const write_datatype_t value) {
                C::write(value);
            }
            /** Set a constant mask of bits in the CSR. */
            template<write_datatype_t MASK> void set_const(void) {
                if constexpr ((MASK & CSR_IMM_OP_MASK) == MASK) {
                    C::set_bits_imm(MASK);
                } else {
                    C::set_bits(MASK);
                }
            }
            /** Set a mask of bits in the CSR. */
            inline void set(write_datatype_t mask) {
                C::set_bits(mask);
            }
            /** Clear a constant mask of bits in the CSR. */
            template<write_datatype_t MASK> void clr_const(void) {
                if constexpr ((MASK & CSR_IMM_OP_MASK) == MASK) {
                    C::clr_bits_imm(MASK);
                } else {
                    C::clr_bits(MASK);
                }
            }
            /** Clear a mask of bits in the CSR. */
            inline void clr(write_datatype_t mask) {
                C::clr_bits(mask);
            }
            /** Operator alias to set mask of bits in the CSR. */
            inline void operator|=(write_datatype_t mask) {
                C::set_bits(mask);
            }
        };
        /** CSR: Read-write base class */
        template<class C> class read_write_reg : public read_only_reg<C>,
                                                 public write_only_reg<C>  {
        public:
            using datatype_t = typename C::datatype;

            read_write_reg(void) 
                : read_only_reg<C>()
                , write_only_reg<C>()
                {}
            read_write_reg(const read_write_reg&)=delete;
            read_write_reg& operator=(const read_write_reg&)=delete;

            /** Read from, then write a constant value to the CSR. */
            template<datatype_t VALUE> datatype_t read_write_const(void) {
                if constexpr ((VALUE & CSR_IMM_OP_MASK) == VALUE) {
                    return C::read_write_imm(VALUE);
                } else {
                    return C::read_write(VALUE);
                }
            }
            /** Read from, then write to the CSR. */
            inline datatype_t read_write(const datatype_t value) {
                return C::read_write(value);
            }
            /** Read from, then set a constant bit mask to the CSR. */
            template<datatype_t MASK> datatype_t read_set_bits_const(void) {
                if constexpr ((MASK & CSR_IMM_OP_MASK) == MASK) {
                    return C::read_set_bits_imm(MASK);
                } else {
                    return C::read_set_bits(MASK);
                }
            }
            /** Read from, then set a bit mask to the CSR. */
            inline datatype_t read_set_bits(const datatype_t mask) {
                return C::read_set_bits(mask);
            }
            /** Read from, then clear a constant bit mask to the CSR. */
            template<datatype_t MASK> datatype_t read_clr_bits_const(void) {
                if constexpr ((MASK & CSR_IMM_OP_MASK) == MASK) {
                    return C::read_clr_bits_imm(MASK);
                } else {
                    return C::read_clr_bits(MASK);
                }
            }
            /** Read from, then clear a bit mask to the CSR. */
            inline datatype_t read_clr_bits(const datatype_t mask) {
                return C::read_clr_bits(mask);
            }
        };
        /** CSR Field: Read only, and read-write base class */
        template<class C, class F> class read_only_field {
        public:
            using read_datatype_t = typename F::datatype;

            read_only_field(void) {}
            read_only_field(const read_only_field&)=delete;
            read_only_field& operator=(const read_only_field&)=delete;

            /** Read a given field value from a CSR */
            read_datatype_t read(void) {
                return (read_datatype_t) ((C::read() & F::BIT_MASK) >> F::BIT_OFFSET);
            }
        };
        /** CSR Field: Write only, and read-write base class */
        template<class C, class F> class write_only_field {
        public:
            using write_datatype_t = typename F::datatype;
            using reg_write_datatype_t = typename C::datatype;

            write_only_field(void) {}
            write_only_field(const write_only_field&)=delete;
            write_only_field& operator=(const write_only_field&)=delete;

            inline void set(void) {
                if constexpr ((F::BIT_MASK & CSR_IMM_OP_MASK) == F::BIT_MASK) {
                    C::set_bits_imm(F::BIT_MASK);
                } else {
                    C::set_bits(F::BIT_MASK);
                }
            }
            inline void clr(void) {
                if constexpr ((F::BIT_MASK & CSR_IMM_OP_MASK) == F::BIT_MASK) {
                    C::clr_bits_imm(F::BIT_MASK);
                } else {
                    C::clr_bits(F::BIT_MASK);
                }
            }
        };
        /** CSR Field: Read-write base class */
        template<class C, class F> class read_write_field 
            : public read_only_field<C,F>
            , public write_only_field<C,F>  {
        public:
            using datatype_t = typename F::datatype;
            using reg_datatype_t = typename C::datatype;

            read_write_field(void) 
                : read_only_field<C,F>()
                , write_only_field<C,F>()
                {}
            read_write_field(const read_write_field&)=delete;
            read_write_field& operator=(const read_write_field&)=delete;

            /* Read-modify-write to write a field.
               NOTE - not atomic.
             */
            inline void write(const datatype_t value) {
                auto org_value = C::read();
                auto new_value = (org_value & ~F::BIT_MASK) | 
                    (((reg_datatype_t)value << F::BIT_OFFSET) & F::BIT_MASK);
                C::write(new_value);
            }
            /* Read-modify-write to set a field, and return original value.
               NOTE - not atomic.
             */
            inline datatype_t read_write(const datatype_t value) {
                auto org_value = C::read();
                auto new_value = (org_value & ~F::BIT_MASK) | 
                    (((reg_datatype_t)value << F::BIT_OFFSET) & F::BIT_MASK);
                C::write(new_value);
                return (datatype_t) ((org_value & F::BIT_MASK) >> F::BIT_OFFSET);
            }

        };

        /** CSR access context and read/write permission.
          */
        typedef enum {            
            URW,
            URO,
            SRW,
            SRO,
            HRW,
            HRO,
            MRW,
            MRO,
            
            DRW,
            DRO,
            } priv_t;

    } /* csr */
} /* riscv */

// ------------------------------------------------------------------------
// Assembler operations and bit field definitions

namespace riscv {
    namespace csr {

        // ----------------------------------------------------------------
        // misa - MRW - Machine ISA 
        //
        /** Machine ISA assembler operations */
        struct misa_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read misa */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, misa" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write misa */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    misa, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to misa */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    misa, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to misa */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, misa, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to misa */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, misa, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for misa */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, misa, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for misa */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, misa, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for misa */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, misa, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for misa */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, misa, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for misa */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, misa, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for misa */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, misa, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for misa */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, misa, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for misa */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, misa, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* misa_ops */

        // ----------------------------------------------------------------
        // mvendorid - MRO - Machine Vendor ID 
        //
        /** Machine Vendor ID assembler operations */
        struct mvendorid_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRO; 
            
            /** Read mvendorid */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mvendorid" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* mvendorid_ops */

        // ----------------------------------------------------------------
        // marchid - MRO - Machine Architecture ID 
        //
        /** Machine Architecture ID assembler operations */
        struct marchid_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRO; 
            
            /** Read marchid */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, marchid" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* marchid_ops */

        // ----------------------------------------------------------------
        // mimpid - MRO - Machine Implementation ID 
        //
        /** Machine Implementation ID assembler operations */
        struct mimpid_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRO; 
            
            /** Read mimpid */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mimpid" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* mimpid_ops */

        // ----------------------------------------------------------------
        // mhartid - MRO - Hardware Thread ID 
        //
        /** Hardware Thread ID assembler operations */
        struct mhartid_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRO; 
            
            /** Read mhartid */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhartid" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* mhartid_ops */

        // ----------------------------------------------------------------
        // mstatus - MRW - Machine Status 
        //
        /** Machine Status assembler operations */
        struct mstatus_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mstatus */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mstatus" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mstatus */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mstatus, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mstatus */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mstatus, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mstatus */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mstatus */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mstatus */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mstatus */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mstatus */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mstatus */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mstatus */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mstatus */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mstatus */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mstatus */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mstatus_ops */
        /** Parameter data for fields in mstatus */
        namespace mstatus_data {
            /** Parameter data for mie */
            struct mie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 3;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x8;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sie */
            struct sie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 2;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x4;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mpie */
            struct mpie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 7;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x80;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for spie */
            struct spie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mprv */
            struct mprv {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 17;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20000;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mpp */
            struct mpp {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 11;
                static constexpr uint_xlen_t BIT_WIDTH  = 2;
                static constexpr uint_xlen_t BIT_MASK   = 0x1800;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x3;
            };
            /** Parameter data for spp */
            struct spp {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* mstatus_data */

        // ----------------------------------------------------------------
        // mstatush - MRW - Additional machine status register, RV32 only. 
        //
        /** Additional machine status register, RV32 only. assembler operations */
        struct mstatush_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mstatush */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mstatush" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mstatush */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mstatush, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mstatush */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mstatush, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mstatush */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mstatush, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mstatush */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mstatush, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mstatush */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mstatush, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mstatush */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mstatush, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mstatush */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mstatush, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mstatush */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mstatush, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mstatush */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mstatush, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mstatush */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mstatush, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mstatush */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mstatush, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mstatush */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mstatush, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mstatush_ops */

        // ----------------------------------------------------------------
        // mtvec - MRW - Machine Trap Vector Base Address 
        //
        /** Machine Trap Vector Base Address assembler operations */
        struct mtvec_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mtvec */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mtvec" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mtvec */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mtvec, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mtvec */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mtvec, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mtvec */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mtvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mtvec */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mtvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mtvec */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mtvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mtvec */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mtvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mtvec */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mtvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mtvec */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mtvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mtvec */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mtvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mtvec */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mtvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mtvec */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mtvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mtvec */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mtvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mtvec_ops */
        /** Parameter data for fields in mtvec */
        namespace mtvec_data {
            /** Parameter data for base */
            struct base {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 2;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-1)-(2) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (2));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (0));
            };
            /** Parameter data for mode */
            struct mode {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 2;
                static constexpr uint_xlen_t BIT_MASK   = 0x3;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x3;
            };
        } /* mtvec_data */

        // ----------------------------------------------------------------
        // medeleg - MRW - Machine Exception Delegation 
        //
        /** Machine Exception Delegation assembler operations */
        struct medeleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read medeleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, medeleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write medeleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    medeleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to medeleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    medeleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to medeleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, medeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to medeleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, medeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for medeleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, medeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for medeleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, medeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for medeleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, medeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for medeleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, medeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for medeleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, medeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for medeleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, medeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for medeleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, medeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for medeleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, medeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* medeleg_ops */

        // ----------------------------------------------------------------
        // mideleg - MRW - Machine Interrupt Delegation 
        //
        /** Machine Interrupt Delegation assembler operations */
        struct mideleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mideleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mideleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mideleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mideleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mideleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mideleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mideleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mideleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mideleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mideleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mideleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mideleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mideleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mideleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mideleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mideleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mideleg_ops */

        // ----------------------------------------------------------------
        // mip - MRW - Machine Interrupt Pending 
        //
        /** Machine Interrupt Pending assembler operations */
        struct mip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mip */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mip, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mip */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mip, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mip */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mip */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mip */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mip */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mip */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mip */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mip */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mip */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mip */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mip */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mip_ops */
        /** Parameter data for fields in mip */
        namespace mip_data {
            /** Parameter data for msi */
            struct msi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 3;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x8;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mti */
            struct mti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 7;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x80;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mei */
            struct mei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 11;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x800;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for ssi */
            struct ssi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 1;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x2;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sti */
            struct sti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sei */
            struct sei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 9;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x200;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for platform_defined */
            struct platform_defined {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 16;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen)-(16) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen)-(16) + 1)-1)) << (16));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen)-(16) + 1)-1)) << (0));
            };
        } /* mip_data */

        // ----------------------------------------------------------------
        // mie - MRW - Machine Interrupt Enable 
        //
        /** Machine Interrupt Enable assembler operations */
        struct mie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mie_ops */
        /** Parameter data for fields in mie */
        namespace mie_data {
            /** Parameter data for msi */
            struct msi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 3;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x8;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mti */
            struct mti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 7;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x80;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for mei */
            struct mei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 11;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x800;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for ssi */
            struct ssi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 1;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x2;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sti */
            struct sti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sei */
            struct sei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 9;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x200;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for platform_defined */
            struct platform_defined {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 16;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen)-(16) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen)-(16) + 1)-1)) << (16));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen)-(16) + 1)-1)) << (0));
            };
        } /* mie_data */

        // ----------------------------------------------------------------
        // mtime -  - Machine Timer 
        //

        // ----------------------------------------------------------------
        // mtimecmp -  - Machine Timer Compare 
        //

        // ----------------------------------------------------------------
        // mcountinhibit - MRW - Machine Counter Inhibit 
        //
        /** Machine Counter Inhibit assembler operations */
        struct mcountinhibit_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mcountinhibit */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mcountinhibit" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mcountinhibit */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mcountinhibit, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mcountinhibit */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mcountinhibit, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mcountinhibit */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mcountinhibit, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mcountinhibit */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mcountinhibit, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mcountinhibit */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mcountinhibit, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mcountinhibit */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mcountinhibit, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mcountinhibit */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mcountinhibit, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mcountinhibit */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mcountinhibit, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mcountinhibit */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mcountinhibit, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mcountinhibit */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mcountinhibit, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mcountinhibit */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mcountinhibit, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mcountinhibit */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mcountinhibit, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mcountinhibit_ops */
        /** Parameter data for fields in mcountinhibit */
        namespace mcountinhibit_data {
            /** Parameter data for cy */
            struct cy {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 0;
                static constexpr uint_csr32_t BIT_WIDTH  = 1;
                static constexpr uint_csr32_t BIT_MASK   = 0x1;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for ir */
            struct ir {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 2;
                static constexpr uint_csr32_t BIT_WIDTH  = 1;
                static constexpr uint_csr32_t BIT_MASK   = 0x4;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for hpm */
            struct hpm {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 3;
                static constexpr uint_csr32_t BIT_WIDTH  = 29;
                static constexpr uint_csr32_t BIT_MASK   = 0xfffffff8;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1fffffff;
            };
        } /* mcountinhibit_data */

        // ----------------------------------------------------------------
        // mcycle - MRW - Clock Cycles Executed Counter 
        //
        /** Clock Cycles Executed Counter assembler operations */
        struct mcycle_ops  {
            using datatype = uint_csr64_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mcycle */
            static uint64_t read(void) {
                uint_csr64_t value;        
                __asm__ volatile ("csrr    %0, mcycle" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mcycle */
            static void write(uint_csr64_t value) {
                __asm__ volatile ("csrw    mcycle, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mcycle */
            static void write_imm(uint_csr64_t value) {
                __asm__ volatile ("csrwi    mcycle, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mcycle */
            static uint64_t read_write(uint_csr64_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrw    %0, mcycle, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mcycle */
            static uint64_t read_write_imm(const uint8_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrwi    %0, mcycle, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mcycle */
            static void set_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrs    zero, mcycle, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mcycle */
            static uint32_t read_set_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrs    %0, mcycle, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mcycle */
            static void clr_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrc    zero, mcycle, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mcycle */
            static uint32_t read_clr_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrc    %0, mcycle, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mcycle */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mcycle, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mcycle */
            static uint64_t read_set_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrsi    %0, mcycle, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mcycle */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mcycle, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mcycle */
            static uint64_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrci    %0, mcycle, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mcycle_ops */

        // ----------------------------------------------------------------
        // minstret - MRW - Number of Instructions Retired Counter 
        //
        /** Number of Instructions Retired Counter assembler operations */
        struct minstret_ops  {
            using datatype = uint_csr64_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read minstret */
            static uint64_t read(void) {
                uint_csr64_t value;        
                __asm__ volatile ("csrr    %0, minstret" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write minstret */
            static void write(uint_csr64_t value) {
                __asm__ volatile ("csrw    minstret, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to minstret */
            static void write_imm(uint_csr64_t value) {
                __asm__ volatile ("csrwi    minstret, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to minstret */
            static uint64_t read_write(uint_csr64_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrw    %0, minstret, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to minstret */
            static uint64_t read_write_imm(const uint8_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrwi    %0, minstret, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for minstret */
            static void set_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrs    zero, minstret, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for minstret */
            static uint32_t read_set_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrs    %0, minstret, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for minstret */
            static void clr_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrc    zero, minstret, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for minstret */
            static uint32_t read_clr_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrc    %0, minstret, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for minstret */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, minstret, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for minstret */
            static uint64_t read_set_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrsi    %0, minstret, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for minstret */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, minstret, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for minstret */
            static uint64_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrci    %0, minstret, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* minstret_ops */

        // ----------------------------------------------------------------
        // mhpmcounter3 - MRW - Event Counters 
        //
        /** Event Counters assembler operations */
        struct mhpmcounter3_ops  {
            using datatype = uint_csr64_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter3 */
            static uint64_t read(void) {
                uint_csr64_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter3" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter3 */
            static void write(uint_csr64_t value) {
                __asm__ volatile ("csrw    mhpmcounter3, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter3 */
            static void write_imm(uint_csr64_t value) {
                __asm__ volatile ("csrwi    mhpmcounter3, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter3 */
            static uint64_t read_write(uint_csr64_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter3 */
            static uint64_t read_write_imm(const uint8_t new_value) {
                uint_csr64_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter3 */
            static void set_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter3 */
            static uint32_t read_set_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter3 */
            static void clr_bits(uint_csr64_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter3 */
            static uint32_t read_clr_bits(uint_csr64_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter3 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter3 */
            static uint64_t read_set_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter3 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter3 */
            static uint64_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr64_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter3_ops */

        // ----------------------------------------------------------------
        // mhpmevent3 - MRW - Event Counter Event Select 
        //
        /** Event Counter Event Select assembler operations */
        struct mhpmevent3_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmevent3 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhpmevent3" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmevent3 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mhpmevent3, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmevent3 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mhpmevent3, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmevent3 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmevent3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmevent3 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmevent3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmevent3 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmevent3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmevent3 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mhpmevent3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmevent3 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmevent3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmevent3 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mhpmevent3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmevent3 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmevent3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmevent3 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mhpmevent3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmevent3 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmevent3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmevent3 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mhpmevent3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmevent3_ops */

        // ----------------------------------------------------------------
        // mcounteren - MRW - Counter Enable 
        //
        /** Counter Enable assembler operations */
        struct mcounteren_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mcounteren */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mcounteren" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mcounteren */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mcounteren, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mcounteren */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mcounteren, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mcounteren */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mcounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mcounteren */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mcounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mcounteren */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mcounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mcounteren */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mcounteren */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mcounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mcounteren */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mcounteren */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mcounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mcounteren */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mcounteren */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mcounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mcounteren */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mcounteren_ops */
        /** Parameter data for fields in mcounteren */
        namespace mcounteren_data {
            /** Parameter data for cy */
            struct cy {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 0;
                static constexpr uint_csr32_t BIT_WIDTH  = 1;
                static constexpr uint_csr32_t BIT_MASK   = 0x1;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for tm */
            struct tm {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 1;
                static constexpr uint_csr32_t BIT_WIDTH  = 1;
                static constexpr uint_csr32_t BIT_MASK   = 0x2;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for ir */
            struct ir {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 2;
                static constexpr uint_csr32_t BIT_WIDTH  = 1;
                static constexpr uint_csr32_t BIT_MASK   = 0x4;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for hpm */
            struct hpm {
                using datatype = uint_xlen_t;
                static constexpr uint_csr32_t BIT_OFFSET = 3;
                static constexpr uint_csr32_t BIT_WIDTH  = 29;
                static constexpr uint_csr32_t BIT_MASK   = 0xfffffff8;
                static constexpr uint_csr32_t ALL_SET_MASK = 0x1fffffff;
            };
        } /* mcounteren_data */

        // ----------------------------------------------------------------
        // scounteren - SRW - Counter Enable 
        //
        /** Counter Enable assembler operations */
        struct scounteren_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read scounteren */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, scounteren" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write scounteren */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    scounteren, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to scounteren */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    scounteren, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to scounteren */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, scounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to scounteren */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, scounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for scounteren */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, scounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for scounteren */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, scounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for scounteren */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, scounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for scounteren */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, scounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for scounteren */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, scounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for scounteren */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, scounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for scounteren */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, scounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for scounteren */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, scounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* scounteren_ops */

        // ----------------------------------------------------------------
        // mscratch - MRW - Machine Mode Scratch Register 
        //
        /** Machine Mode Scratch Register assembler operations */
        struct mscratch_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mscratch */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mscratch" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mscratch */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mscratch, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mscratch */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mscratch, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mscratch */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mscratch */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mscratch */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mscratch */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mscratch */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mscratch */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mscratch */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mscratch */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mscratch */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mscratch */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mscratch_ops */

        // ----------------------------------------------------------------
        // mepc - MRW - Machine Exception Program Counter 
        //
        /** Machine Exception Program Counter assembler operations */
        struct mepc_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mepc */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mepc" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mepc */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mepc, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mepc */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mepc, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mepc */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mepc */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mepc */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mepc */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mepc */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mepc */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mepc */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mepc */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mepc */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mepc */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mepc_ops */

        // ----------------------------------------------------------------
        // mcause - MRW - Machine Exception Cause 
        //
        /** Machine Exception Cause assembler operations */
        struct mcause_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mcause */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mcause" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mcause */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mcause, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mcause */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mcause, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mcause */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mcause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mcause */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mcause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mcause */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mcause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mcause */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mcause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mcause */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mcause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mcause */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mcause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mcause */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mcause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mcause */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mcause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mcause */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mcause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mcause */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mcause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mcause_ops */
        /** Parameter data for fields in mcause */
        namespace mcause_data {
            /** Parameter data for interrupt */
            struct interrupt {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = (__riscv_xlen-1);
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = (0x1UL << ((__riscv_xlen-1)));
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for exception_code */
            struct exception_code {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-2)-(0) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
            };
        } /* mcause_data */

        // ----------------------------------------------------------------
        // mtval - MRW - Machine Trap Value 
        //
        /** Machine Trap Value assembler operations */
        struct mtval_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mtval */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mtval" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mtval */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mtval, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mtval */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mtval, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mtval */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mtval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mtval */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mtval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mtval */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mtval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mtval */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mtval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mtval */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mtval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mtval */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mtval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mtval */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mtval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mtval */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mtval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mtval */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mtval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mtval */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mtval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mtval_ops */

        // ----------------------------------------------------------------
        // sscratch - SRW - Supervisor Mode Scratch Register 
        //
        /** Supervisor Mode Scratch Register assembler operations */
        struct sscratch_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sscratch */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sscratch" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sscratch */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sscratch, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sscratch */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sscratch, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sscratch */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sscratch */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sscratch */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sscratch */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sscratch */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sscratch */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sscratch */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sscratch */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sscratch */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sscratch */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sscratch_ops */

        // ----------------------------------------------------------------
        // sepc - SRW - Supervisor Exception Program Counter 
        //
        /** Supervisor Exception Program Counter assembler operations */
        struct sepc_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sepc */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sepc" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sepc */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sepc, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sepc */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sepc, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sepc */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sepc */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sepc */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sepc */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sepc */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sepc */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sepc */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sepc */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sepc */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sepc */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sepc_ops */

        // ----------------------------------------------------------------
        // scause - SRW - Supervisor Exception Cause 
        //
        /** Supervisor Exception Cause assembler operations */
        struct scause_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read scause */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, scause" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write scause */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    scause, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to scause */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    scause, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to scause */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, scause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to scause */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, scause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for scause */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, scause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for scause */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, scause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for scause */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, scause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for scause */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, scause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for scause */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, scause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for scause */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, scause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for scause */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, scause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for scause */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, scause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* scause_ops */
        /** Parameter data for fields in scause */
        namespace scause_data {
            /** Parameter data for interrupt */
            struct interrupt {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = (__riscv_xlen-1);
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = (0x1UL << ((__riscv_xlen-1)));
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for exception_code */
            struct exception_code {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-2)-(0) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
            };
        } /* scause_data */

        // ----------------------------------------------------------------
        // sstatus - SRW - Supervisor Status 
        //
        /** Supervisor Status assembler operations */
        struct sstatus_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sstatus */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sstatus" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sstatus */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sstatus, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sstatus */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sstatus, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sstatus */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sstatus */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sstatus */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sstatus */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sstatus */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sstatus */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sstatus */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sstatus */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sstatus */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sstatus */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sstatus_ops */
        /** Parameter data for fields in sstatus */
        namespace sstatus_data {
            /** Parameter data for sie */
            struct sie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 2;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x4;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for spie */
            struct spie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for spp */
            struct spp {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* sstatus_data */

        // ----------------------------------------------------------------
        // stvec - SRW - Supervisor Trap Vector Base Address 
        //
        /** Supervisor Trap Vector Base Address assembler operations */
        struct stvec_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read stvec */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, stvec" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write stvec */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    stvec, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to stvec */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    stvec, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to stvec */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, stvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to stvec */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, stvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for stvec */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, stvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for stvec */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, stvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for stvec */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, stvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for stvec */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, stvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for stvec */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, stvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for stvec */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, stvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for stvec */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, stvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for stvec */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, stvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* stvec_ops */
        /** Parameter data for fields in stvec */
        namespace stvec_data {
            /** Parameter data for base */
            struct base {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 2;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-1)-(2) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (2));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (0));
            };
            /** Parameter data for mode */
            struct mode {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 2;
                static constexpr uint_xlen_t BIT_MASK   = 0x3;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x3;
            };
        } /* stvec_data */

        // ----------------------------------------------------------------
        // sideleg - SRW - Supervisor Interrupt Delegation 
        //
        /** Supervisor Interrupt Delegation assembler operations */
        struct sideleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sideleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sideleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sideleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sideleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sideleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sideleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sideleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sideleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sideleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sideleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sideleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sideleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sideleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sideleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sideleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sideleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sideleg_ops */

        // ----------------------------------------------------------------
        // sedeleg - SRW - Supervisor Exception Delegation 
        //
        /** Supervisor Exception Delegation assembler operations */
        struct sedeleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sedeleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sedeleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sedeleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sedeleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sedeleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sedeleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sedeleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sedeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sedeleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sedeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sedeleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sedeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sedeleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sedeleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sedeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sedeleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sedeleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sedeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sedeleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sedeleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sedeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sedeleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sedeleg_ops */

        // ----------------------------------------------------------------
        // sip - SRW - Supervisor Interrupt Pending 
        //
        /** Supervisor Interrupt Pending assembler operations */
        struct sip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sip */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sip, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sip */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sip, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sip */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sip */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sip */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sip */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sip */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sip */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sip */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sip */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sip */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sip */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sip_ops */
        /** Parameter data for fields in sip */
        namespace sip_data {
            /** Parameter data for ssi */
            struct ssi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 1;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x2;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sti */
            struct sti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sei */
            struct sei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 9;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x200;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* sip_data */

        // ----------------------------------------------------------------
        // sie - SRW - Supervisor Interrupt Enable 
        //
        /** Supervisor Interrupt Enable assembler operations */
        struct sie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read sie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, sie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write sie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    sie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to sie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    sie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to sie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, sie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to sie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, sie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for sie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, sie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for sie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, sie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for sie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, sie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for sie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, sie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for sie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, sie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for sie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, sie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for sie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, sie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for sie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, sie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* sie_ops */
        /** Parameter data for fields in sie */
        namespace sie_data {
            /** Parameter data for ssi */
            struct ssi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 1;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x2;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sti */
            struct sti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 5;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x20;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for sei */
            struct sei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 9;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x200;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* sie_data */

        // ----------------------------------------------------------------
        // ustatus - URW - User mode restricted view of mstatus 
        //
        /** User mode restricted view of mstatus assembler operations */
        struct ustatus_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read ustatus */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, ustatus" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write ustatus */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    ustatus, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to ustatus */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    ustatus, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to ustatus */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, ustatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to ustatus */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, ustatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for ustatus */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, ustatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for ustatus */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, ustatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for ustatus */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, ustatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for ustatus */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, ustatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for ustatus */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, ustatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for ustatus */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, ustatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for ustatus */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, ustatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for ustatus */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, ustatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* ustatus_ops */
        /** Parameter data for fields in ustatus */
        namespace ustatus_data {
            /** Parameter data for uie */
            struct uie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 1;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x2;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for upie */
            struct upie {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 3;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x8;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* ustatus_data */

        // ----------------------------------------------------------------
        // uip - URW - User Interrupt Pending 
        //
        /** User Interrupt Pending assembler operations */
        struct uip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read uip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, uip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write uip */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    uip, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to uip */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    uip, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to uip */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, uip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to uip */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, uip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for uip */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, uip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for uip */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, uip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for uip */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, uip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for uip */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, uip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for uip */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, uip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for uip */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, uip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for uip */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, uip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for uip */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, uip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* uip_ops */
        /** Parameter data for fields in uip */
        namespace uip_data {
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* uip_data */

        // ----------------------------------------------------------------
        // uie - URW - User Interrupt Enable 
        //
        /** User Interrupt Enable assembler operations */
        struct uie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read uie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, uie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write uie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    uie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to uie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    uie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to uie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, uie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to uie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, uie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for uie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, uie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for uie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, uie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for uie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, uie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for uie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, uie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for uie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, uie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for uie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, uie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for uie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, uie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for uie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, uie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* uie_ops */
        /** Parameter data for fields in uie */
        namespace uie_data {
            /** Parameter data for usi */
            struct usi {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x1;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uti */
            struct uti {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 4;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x10;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for uei */
            struct uei {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 8;
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = 0x100;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
        } /* uie_data */

        // ----------------------------------------------------------------
        // uscratch - URW - User Mode Scratch Register 
        //
        /** User Mode Scratch Register assembler operations */
        struct uscratch_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read uscratch */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, uscratch" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write uscratch */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    uscratch, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to uscratch */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    uscratch, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to uscratch */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, uscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to uscratch */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, uscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for uscratch */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, uscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for uscratch */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, uscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for uscratch */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, uscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for uscratch */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, uscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for uscratch */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, uscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for uscratch */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, uscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for uscratch */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, uscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for uscratch */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, uscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* uscratch_ops */

        // ----------------------------------------------------------------
        // uepc - URW - User Exception Program Counter 
        //
        /** User Exception Program Counter assembler operations */
        struct uepc_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read uepc */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, uepc" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write uepc */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    uepc, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to uepc */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    uepc, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to uepc */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, uepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to uepc */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, uepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for uepc */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, uepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for uepc */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, uepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for uepc */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, uepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for uepc */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, uepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for uepc */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, uepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for uepc */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, uepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for uepc */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, uepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for uepc */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, uepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* uepc_ops */

        // ----------------------------------------------------------------
        // ucause - URW - User Exception Cause 
        //
        /** User Exception Cause assembler operations */
        struct ucause_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read ucause */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, ucause" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write ucause */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    ucause, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to ucause */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    ucause, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to ucause */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, ucause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to ucause */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, ucause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for ucause */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, ucause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for ucause */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, ucause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for ucause */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, ucause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for ucause */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, ucause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for ucause */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, ucause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for ucause */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, ucause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for ucause */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, ucause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for ucause */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, ucause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* ucause_ops */
        /** Parameter data for fields in ucause */
        namespace ucause_data {
            /** Parameter data for interrupt */
            struct interrupt {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = (__riscv_xlen-1);
                static constexpr uint_xlen_t BIT_WIDTH  = 1;
                static constexpr uint_xlen_t BIT_MASK   = (0x1UL << ((__riscv_xlen-1)));
                static constexpr uint_xlen_t ALL_SET_MASK = 0x1;
            };
            /** Parameter data for exception_code */
            struct exception_code {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-2)-(0) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-2)-(0) + 1)-1)) << (0));
            };
        } /* ucause_data */

        // ----------------------------------------------------------------
        // utvec - URW - User Trap Vector Base Address 
        //
        /** User Trap Vector Base Address assembler operations */
        struct utvec_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read utvec */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, utvec" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write utvec */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    utvec, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to utvec */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    utvec, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to utvec */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, utvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to utvec */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, utvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for utvec */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, utvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for utvec */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, utvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for utvec */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, utvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for utvec */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, utvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for utvec */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, utvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for utvec */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, utvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for utvec */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, utvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for utvec */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, utvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* utvec_ops */
        /** Parameter data for fields in utvec */
        namespace utvec_data {
            /** Parameter data for base */
            struct base {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 2;
                static constexpr uint_xlen_t BIT_WIDTH  = ((__riscv_xlen-1)-(2) + 1);
                static constexpr uint_xlen_t BIT_MASK   = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (2));
                static constexpr uint_xlen_t ALL_SET_MASK = ((1UL<<(((__riscv_xlen-1)-(2) + 1)-1)) << (0));
            };
            /** Parameter data for mode */
            struct mode {
                using datatype = uint_xlen_t;
                static constexpr uint_xlen_t BIT_OFFSET = 0;
                static constexpr uint_xlen_t BIT_WIDTH  = 2;
                static constexpr uint_xlen_t BIT_MASK   = 0x3;
                static constexpr uint_xlen_t ALL_SET_MASK = 0x3;
            };
        } /* utvec_data */

        // ----------------------------------------------------------------
        // utval - URW - User Trap Value 
        //
        /** User Trap Value assembler operations */
        struct utval_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read utval */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, utval" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write utval */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    utval, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to utval */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    utval, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to utval */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, utval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to utval */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, utval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for utval */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, utval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for utval */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, utval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for utval */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, utval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for utval */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, utval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for utval */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, utval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for utval */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, utval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for utval */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, utval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for utval */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, utval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* utval_ops */

        // ----------------------------------------------------------------
        // fflags - URW - Floating-Point Accrued Exceptions. 
        //
        /** Floating-Point Accrued Exceptions. assembler operations */
        struct fflags_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read fflags */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, fflags" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write fflags */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    fflags, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to fflags */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    fflags, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to fflags */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, fflags, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to fflags */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, fflags, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for fflags */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, fflags, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for fflags */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, fflags, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for fflags */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, fflags, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for fflags */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, fflags, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for fflags */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, fflags, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for fflags */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, fflags, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for fflags */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, fflags, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for fflags */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, fflags, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* fflags_ops */

        // ----------------------------------------------------------------
        // frm - URW - Floating-Point Dynamic Rounding Mode. 
        //
        /** Floating-Point Dynamic Rounding Mode. assembler operations */
        struct frm_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read frm */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, frm" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write frm */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    frm, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to frm */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    frm, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to frm */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, frm, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to frm */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, frm, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for frm */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, frm, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for frm */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, frm, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for frm */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, frm, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for frm */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, frm, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for frm */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, frm, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for frm */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, frm, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for frm */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, frm, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for frm */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, frm, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* frm_ops */

        // ----------------------------------------------------------------
        // fcsr - URW - Floating-Point Control and Status 
        //
        /** Floating-Point Control and Status assembler operations */
        struct fcsr_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URW; 
            
            /** Read fcsr */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, fcsr" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write fcsr */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    fcsr, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to fcsr */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    fcsr, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to fcsr */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, fcsr, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to fcsr */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, fcsr, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for fcsr */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, fcsr, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for fcsr */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, fcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for fcsr */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, fcsr, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for fcsr */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, fcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for fcsr */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, fcsr, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for fcsr */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, fcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for fcsr */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, fcsr, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for fcsr */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, fcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* fcsr_ops */

        // ----------------------------------------------------------------
        // cycle - URO - Cycle counter for RDCYCLE instruction. 
        //
        /** Cycle counter for RDCYCLE instruction. assembler operations */
        struct cycle_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read cycle */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, cycle" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* cycle_ops */

        // ----------------------------------------------------------------
        // time - URO - Timer for RDTIME instruction. 
        //
        /** Timer for RDTIME instruction. assembler operations */
        struct time_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read time */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, time" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* time_ops */

        // ----------------------------------------------------------------
        // instret - URO - Instructions-retired counter for RDINSTRET instruction. 
        //
        /** Instructions-retired counter for RDINSTRET instruction. assembler operations */
        struct instret_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read instret */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, instret" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* instret_ops */

        // ----------------------------------------------------------------
        // hpmcounter3 - URO - Performance-monitoring counter. 
        //
        /** Performance-monitoring counter. assembler operations */
        struct hpmcounter3_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter3 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter3" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter3_ops */

        // ----------------------------------------------------------------
        // hpmcounter4 - URO - Performance-monitoring counter. 
        //
        /** Performance-monitoring counter. assembler operations */
        struct hpmcounter4_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter4 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter4" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter4_ops */

        // ----------------------------------------------------------------
        // hpmcounter31 - URO - Performance-monitoring counter. 
        //
        /** Performance-monitoring counter. assembler operations */
        struct hpmcounter31_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter31 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter31" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter31_ops */

        // ----------------------------------------------------------------
        // cycleh - URO - Upper 32 bits of  cycle, RV32I only. 
        //
        /** Upper 32 bits of  cycle, RV32I only. assembler operations */
        struct cycleh_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read cycleh */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, cycleh" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* cycleh_ops */

        // ----------------------------------------------------------------
        // timeh - URO - Upper 32 bits of  time, RV32I only. 
        //
        /** Upper 32 bits of  time, RV32I only. assembler operations */
        struct timeh_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read timeh */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, timeh" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* timeh_ops */

        // ----------------------------------------------------------------
        // instreth - URO - Upper 32 bits of  instret, RV32I only. 
        //
        /** Upper 32 bits of  instret, RV32I only. assembler operations */
        struct instreth_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read instreth */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, instreth" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* instreth_ops */

        // ----------------------------------------------------------------
        // hpmcounter3h - URO - Upper 32 bits of  hpmcounter3, RV32I only. 
        //
        /** Upper 32 bits of  hpmcounter3, RV32I only. assembler operations */
        struct hpmcounter3h_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter3h */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter3h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter3h_ops */

        // ----------------------------------------------------------------
        // hpmcounter4h - URO - Upper 32 bits of  hpmcounter4, RV32I only. 
        //
        /** Upper 32 bits of  hpmcounter4, RV32I only. assembler operations */
        struct hpmcounter4h_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter4h */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter4h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter4h_ops */

        // ----------------------------------------------------------------
        // hpmcounter31h - URO - Upper 32 bits of  hpmcounter31, RV32I only. 
        //
        /** Upper 32 bits of  hpmcounter31, RV32I only. assembler operations */
        struct hpmcounter31h_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = URO; 
            
            /** Read hpmcounter31h */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hpmcounter31h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hpmcounter31h_ops */

        // ----------------------------------------------------------------
        // stval - SRW - Supervisor bad address or instruction. 
        //
        /** Supervisor bad address or instruction. assembler operations */
        struct stval_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read stval */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, stval" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write stval */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    stval, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to stval */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    stval, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to stval */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, stval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to stval */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, stval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for stval */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, stval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for stval */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, stval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for stval */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, stval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for stval */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, stval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for stval */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, stval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for stval */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, stval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for stval */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, stval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for stval */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, stval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* stval_ops */

        // ----------------------------------------------------------------
        // satp - SRW - Supervisor address translation and protection. 
        //
        /** Supervisor address translation and protection. assembler operations */
        struct satp_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = SRW; 
            
            /** Read satp */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, satp" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write satp */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    satp, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to satp */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    satp, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to satp */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, satp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to satp */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, satp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for satp */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, satp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for satp */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, satp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for satp */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, satp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for satp */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, satp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for satp */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, satp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for satp */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, satp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for satp */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, satp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for satp */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, satp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* satp_ops */

        // ----------------------------------------------------------------
        // hstatus - HRW - Hypervisor status register. 
        //
        /** Hypervisor status register. assembler operations */
        struct hstatus_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hstatus */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hstatus" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hstatus */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hstatus, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hstatus */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hstatus, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hstatus */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hstatus */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hstatus */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hstatus */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hstatus */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hstatus */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hstatus */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hstatus */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hstatus */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hstatus */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hstatus_ops */

        // ----------------------------------------------------------------
        // hedeleg - HRW - Hypervisor exception delegation register. 
        //
        /** Hypervisor exception delegation register. assembler operations */
        struct hedeleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hedeleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hedeleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hedeleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hedeleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hedeleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hedeleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hedeleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hedeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hedeleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hedeleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hedeleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hedeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hedeleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hedeleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hedeleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hedeleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hedeleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hedeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hedeleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hedeleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hedeleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hedeleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hedeleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hedeleg_ops */

        // ----------------------------------------------------------------
        // hideleg - HRW - Hypervisor interrupt delegation register. 
        //
        /** Hypervisor interrupt delegation register. assembler operations */
        struct hideleg_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hideleg */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hideleg" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hideleg */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hideleg, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hideleg */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hideleg, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hideleg */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hideleg */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hideleg, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hideleg */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hideleg */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hideleg */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hideleg, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hideleg */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hideleg */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hideleg */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hideleg */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hideleg, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hideleg */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hideleg, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hideleg_ops */

        // ----------------------------------------------------------------
        // hcounteren - HRW - Hypervisor counter enable. 
        //
        /** Hypervisor counter enable. assembler operations */
        struct hcounteren_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hcounteren */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hcounteren" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hcounteren */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hcounteren, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hcounteren */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hcounteren, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hcounteren */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hcounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hcounteren */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hcounteren, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hcounteren */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hcounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hcounteren */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hcounteren */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hcounteren, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hcounteren */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hcounteren */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hcounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hcounteren */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hcounteren */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hcounteren, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hcounteren */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hcounteren, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hcounteren_ops */

        // ----------------------------------------------------------------
        // hgatp - HRW - Hypervisor guest address translation and protection. 
        //
        /** Hypervisor guest address translation and protection. assembler operations */
        struct hgatp_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hgatp */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hgatp" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hgatp */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hgatp, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hgatp */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hgatp, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hgatp */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hgatp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hgatp */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hgatp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hgatp */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hgatp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hgatp */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hgatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hgatp */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hgatp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hgatp */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hgatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hgatp */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hgatp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hgatp */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hgatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hgatp */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hgatp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hgatp */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hgatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hgatp_ops */

        // ----------------------------------------------------------------
        // htimedelta - HRW - Delta for VS/VU-mode timer. 
        //
        /** Delta for VS/VU-mode timer. assembler operations */
        struct htimedelta_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read htimedelta */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, htimedelta" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write htimedelta */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    htimedelta, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to htimedelta */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    htimedelta, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to htimedelta */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, htimedelta, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to htimedelta */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, htimedelta, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for htimedelta */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, htimedelta, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for htimedelta */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, htimedelta, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for htimedelta */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, htimedelta, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for htimedelta */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, htimedelta, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for htimedelta */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, htimedelta, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for htimedelta */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, htimedelta, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for htimedelta */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, htimedelta, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for htimedelta */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, htimedelta, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* htimedelta_ops */

        // ----------------------------------------------------------------
        // htimedeltah - HRW - Upper 32 bits of  htimedelta, RV32I only. 
        //
        /** Upper 32 bits of  htimedelta, RV32I only. assembler operations */
        struct htimedeltah_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read htimedeltah */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, htimedeltah" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write htimedeltah */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    htimedeltah, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to htimedeltah */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    htimedeltah, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to htimedeltah */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, htimedeltah, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to htimedeltah */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, htimedeltah, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for htimedeltah */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, htimedeltah, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for htimedeltah */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, htimedeltah, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for htimedeltah */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, htimedeltah, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for htimedeltah */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, htimedeltah, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for htimedeltah */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, htimedeltah, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for htimedeltah */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, htimedeltah, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for htimedeltah */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, htimedeltah, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for htimedeltah */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, htimedeltah, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* htimedeltah_ops */

        // ----------------------------------------------------------------
        // vsstatus - HRW - Virtual supervisor status register. 
        //
        /** Virtual supervisor status register. assembler operations */
        struct vsstatus_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsstatus */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsstatus" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsstatus */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsstatus, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsstatus */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsstatus, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsstatus */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsstatus */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsstatus, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsstatus */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsstatus */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsstatus */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsstatus, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsstatus */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsstatus */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsstatus */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsstatus */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsstatus, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsstatus */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsstatus, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsstatus_ops */

        // ----------------------------------------------------------------
        // vsie - HRW - Virtual supervisor interrupt-enable register. 
        //
        /** Virtual supervisor interrupt-enable register. assembler operations */
        struct vsie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsie_ops */

        // ----------------------------------------------------------------
        // vstvec - HRW - Virtual supervisor trap handler base address. 
        //
        /** Virtual supervisor trap handler base address. assembler operations */
        struct vstvec_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vstvec */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vstvec" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vstvec */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vstvec, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vstvec */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vstvec, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vstvec */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vstvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vstvec */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vstvec, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vstvec */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vstvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vstvec */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vstvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vstvec */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vstvec, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vstvec */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vstvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vstvec */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vstvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vstvec */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vstvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vstvec */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vstvec, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vstvec */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vstvec, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vstvec_ops */

        // ----------------------------------------------------------------
        // vsscratch - HRW - Virtual supervisor scratch register. 
        //
        /** Virtual supervisor scratch register. assembler operations */
        struct vsscratch_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsscratch */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsscratch" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsscratch */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsscratch, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsscratch */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsscratch, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsscratch */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsscratch */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsscratch, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsscratch */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsscratch */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsscratch */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsscratch, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsscratch */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsscratch */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsscratch */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsscratch */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsscratch, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsscratch */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsscratch, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsscratch_ops */

        // ----------------------------------------------------------------
        // vsepc - HRW - Virtual supervisor exception program counter. 
        //
        /** Virtual supervisor exception program counter. assembler operations */
        struct vsepc_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsepc */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsepc" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsepc */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsepc, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsepc */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsepc, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsepc */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsepc */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsepc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsepc */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsepc */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsepc */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsepc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsepc */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsepc */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsepc */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsepc */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsepc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsepc */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsepc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsepc_ops */

        // ----------------------------------------------------------------
        // vscause - HRW - Virtual supervisor trap cause. 
        //
        /** Virtual supervisor trap cause. assembler operations */
        struct vscause_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vscause */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vscause" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vscause */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vscause, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vscause */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vscause, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vscause */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vscause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vscause */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vscause, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vscause */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vscause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vscause */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vscause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vscause */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vscause, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vscause */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vscause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vscause */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vscause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vscause */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vscause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vscause */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vscause, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vscause */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vscause, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vscause_ops */

        // ----------------------------------------------------------------
        // vstval - HRW - Virtual supervisor bad address or instruction. 
        //
        /** Virtual supervisor bad address or instruction. assembler operations */
        struct vstval_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vstval */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vstval" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vstval */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vstval, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vstval */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vstval, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vstval */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vstval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vstval */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vstval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vstval */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vstval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vstval */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vstval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vstval */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vstval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vstval */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vstval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vstval */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vstval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vstval */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vstval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vstval */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vstval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vstval */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vstval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vstval_ops */

        // ----------------------------------------------------------------
        // vsip - HRW - Virtual supervisor interrupt pending. 
        //
        /** Virtual supervisor interrupt pending. assembler operations */
        struct vsip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsip */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsip, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsip */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsip, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsip */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsip */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsip */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsip */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsip */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsip */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsip */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsip */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsip */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsip */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsip_ops */

        // ----------------------------------------------------------------
        // vsatp - HRW - Virtual supervisor address translation and protection. 
        //
        /** Virtual supervisor address translation and protection. assembler operations */
        struct vsatp_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read vsatp */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, vsatp" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write vsatp */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    vsatp, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to vsatp */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    vsatp, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to vsatp */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, vsatp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to vsatp */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, vsatp, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for vsatp */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, vsatp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for vsatp */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, vsatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for vsatp */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, vsatp, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for vsatp */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, vsatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for vsatp */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, vsatp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for vsatp */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, vsatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for vsatp */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, vsatp, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for vsatp */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, vsatp, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* vsatp_ops */

        // ----------------------------------------------------------------
        // mbase - MRW - Base register. 
        //
        /** Base register. assembler operations */
        struct mbase_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mbase */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mbase" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mbase */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mbase, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mbase */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mbase, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mbase */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mbase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mbase */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mbase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mbase */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mbase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mbase */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mbase */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mbase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mbase */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mbase */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mbase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mbase */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mbase */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mbase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mbase */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mbase_ops */

        // ----------------------------------------------------------------
        // mbound - MRW - Bound register. 
        //
        /** Bound register. assembler operations */
        struct mbound_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mbound */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mbound" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mbound */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mbound, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mbound */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mbound, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mbound */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mbound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mbound */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mbound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mbound */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mbound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mbound */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mbound */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mbound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mbound */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mbound */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mbound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mbound */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mbound */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mbound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mbound */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mbound_ops */

        // ----------------------------------------------------------------
        // mibase - MRW - Instruction base register. 
        //
        /** Instruction base register. assembler operations */
        struct mibase_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mibase */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mibase" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mibase */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mibase, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mibase */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mibase, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mibase */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mibase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mibase */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mibase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mibase */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mibase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mibase */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mibase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mibase */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mibase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mibase */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mibase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mibase */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mibase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mibase */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mibase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mibase */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mibase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mibase */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mibase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mibase_ops */

        // ----------------------------------------------------------------
        // mibound - MRW - Instruction bound register. 
        //
        /** Instruction bound register. assembler operations */
        struct mibound_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mibound */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mibound" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mibound */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mibound, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mibound */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mibound, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mibound */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mibound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mibound */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mibound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mibound */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mibound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mibound */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mibound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mibound */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mibound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mibound */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mibound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mibound */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mibound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mibound */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mibound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mibound */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mibound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mibound */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mibound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mibound_ops */

        // ----------------------------------------------------------------
        // mdbase - MRW - Data base register. 
        //
        /** Data base register. assembler operations */
        struct mdbase_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mdbase */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mdbase" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mdbase */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mdbase, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mdbase */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mdbase, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mdbase */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mdbase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mdbase */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mdbase, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mdbase */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mdbase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mdbase */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mdbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mdbase */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mdbase, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mdbase */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mdbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mdbase */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mdbase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mdbase */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mdbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mdbase */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mdbase, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mdbase */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mdbase, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mdbase_ops */

        // ----------------------------------------------------------------
        // mdbound - MRW - Data bound register. 
        //
        /** Data bound register. assembler operations */
        struct mdbound_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mdbound */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mdbound" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mdbound */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mdbound, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mdbound */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mdbound, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mdbound */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mdbound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mdbound */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mdbound, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mdbound */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mdbound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mdbound */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mdbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mdbound */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mdbound, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mdbound */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mdbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mdbound */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mdbound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mdbound */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mdbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mdbound */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mdbound, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mdbound */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mdbound, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mdbound_ops */

        // ----------------------------------------------------------------
        // pmpcfg0 - MRW - Physical memory protection configuration. 
        //
        /** Physical memory protection configuration. assembler operations */
        struct pmpcfg0_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpcfg0 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpcfg0" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpcfg0 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpcfg0, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpcfg0 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpcfg0, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpcfg0 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpcfg0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpcfg0 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpcfg0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpcfg0 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpcfg0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpcfg0 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpcfg0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpcfg0 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpcfg0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpcfg0 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpcfg0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpcfg0 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpcfg0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpcfg0 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpcfg0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpcfg0 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpcfg0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpcfg0 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpcfg0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpcfg0_ops */

        // ----------------------------------------------------------------
        // pmpcfg1 - MRW - Physical memory protection configuration, RV32 only. 
        //
        /** Physical memory protection configuration, RV32 only. assembler operations */
        struct pmpcfg1_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpcfg1 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpcfg1" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpcfg1 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpcfg1, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpcfg1 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpcfg1, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpcfg1 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpcfg1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpcfg1 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpcfg1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpcfg1 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpcfg1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpcfg1 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpcfg1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpcfg1 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpcfg1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpcfg1 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpcfg1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpcfg1 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpcfg1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpcfg1 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpcfg1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpcfg1 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpcfg1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpcfg1 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpcfg1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpcfg1_ops */

        // ----------------------------------------------------------------
        // pmpcfg2 - MRW - Physical memory protection configuration. 
        //
        /** Physical memory protection configuration. assembler operations */
        struct pmpcfg2_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpcfg2 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpcfg2" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpcfg2 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpcfg2, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpcfg2 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpcfg2, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpcfg2 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpcfg2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpcfg2 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpcfg2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpcfg2 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpcfg2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpcfg2 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpcfg2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpcfg2 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpcfg2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpcfg2 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpcfg2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpcfg2 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpcfg2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpcfg2 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpcfg2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpcfg2 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpcfg2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpcfg2 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpcfg2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpcfg2_ops */

        // ----------------------------------------------------------------
        // pmpcfg3 - MRW - Physical memory protection configuration, RV32 only. 
        //
        /** Physical memory protection configuration, RV32 only. assembler operations */
        struct pmpcfg3_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpcfg3 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpcfg3" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpcfg3 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpcfg3, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpcfg3 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpcfg3, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpcfg3 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpcfg3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpcfg3 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpcfg3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpcfg3 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpcfg3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpcfg3 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpcfg3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpcfg3 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpcfg3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpcfg3 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpcfg3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpcfg3 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpcfg3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpcfg3 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpcfg3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpcfg3 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpcfg3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpcfg3 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpcfg3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpcfg3_ops */

        // ----------------------------------------------------------------
        // pmpaddr0 - MRW - Physical memory protection address register. 
        //
        /** Physical memory protection address register. assembler operations */
        struct pmpaddr0_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpaddr0 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpaddr0" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpaddr0 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpaddr0, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpaddr0 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpaddr0, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpaddr0 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpaddr0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpaddr0 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpaddr0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpaddr0 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpaddr0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpaddr0 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpaddr0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpaddr0 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpaddr0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpaddr0 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpaddr0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpaddr0 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpaddr0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpaddr0 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpaddr0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpaddr0 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpaddr0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpaddr0 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpaddr0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpaddr0_ops */

        // ----------------------------------------------------------------
        // pmpaddr1 - MRW - Physical memory protection address register. 
        //
        /** Physical memory protection address register. assembler operations */
        struct pmpaddr1_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpaddr1 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpaddr1" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpaddr1 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpaddr1, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpaddr1 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpaddr1, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpaddr1 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpaddr1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpaddr1 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpaddr1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpaddr1 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpaddr1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpaddr1 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpaddr1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpaddr1 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpaddr1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpaddr1 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpaddr1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpaddr1 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpaddr1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpaddr1 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpaddr1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpaddr1 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpaddr1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpaddr1 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpaddr1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpaddr1_ops */

        // ----------------------------------------------------------------
        // pmpaddr15 - MRW - Physical memory protection address register. 
        //
        /** Physical memory protection address register. assembler operations */
        struct pmpaddr15_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read pmpaddr15 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, pmpaddr15" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write pmpaddr15 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    pmpaddr15, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to pmpaddr15 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    pmpaddr15, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to pmpaddr15 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, pmpaddr15, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to pmpaddr15 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, pmpaddr15, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for pmpaddr15 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, pmpaddr15, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for pmpaddr15 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, pmpaddr15, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for pmpaddr15 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, pmpaddr15, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for pmpaddr15 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, pmpaddr15, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for pmpaddr15 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, pmpaddr15, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for pmpaddr15 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, pmpaddr15, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for pmpaddr15 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, pmpaddr15, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for pmpaddr15 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, pmpaddr15, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* pmpaddr15_ops */

        // ----------------------------------------------------------------
        // mhpmcounter4 - MRW - Machine performance-monitoring counter. 
        //
        /** Machine performance-monitoring counter. assembler operations */
        struct mhpmcounter4_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter4 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter4" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter4 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mhpmcounter4, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter4 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mhpmcounter4, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter4 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter4, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter4 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter4, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter4 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter4, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter4 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter4 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter4, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter4 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter4 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter4, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter4 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter4 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter4, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter4 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter4_ops */

        // ----------------------------------------------------------------
        // mhpmcounter31 - MRW - Machine performance-monitoring counter. 
        //
        /** Machine performance-monitoring counter. assembler operations */
        struct mhpmcounter31_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter31 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter31" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter31 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mhpmcounter31, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter31 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mhpmcounter31, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter31 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter31, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter31 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter31, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter31 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter31, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter31 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter31 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter31, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter31 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter31 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter31, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter31 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter31 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter31, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter31 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter31_ops */

        // ----------------------------------------------------------------
        // mcycleh - MRW - Upper 32 bits of  mcycle, RV32I only. 
        //
        /** Upper 32 bits of  mcycle, RV32I only. assembler operations */
        struct mcycleh_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mcycleh */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mcycleh" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mcycleh */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mcycleh, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mcycleh */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mcycleh, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mcycleh */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mcycleh, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mcycleh */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mcycleh, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mcycleh */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mcycleh, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mcycleh */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mcycleh, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mcycleh */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mcycleh, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mcycleh */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mcycleh, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mcycleh */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mcycleh, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mcycleh */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mcycleh, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mcycleh */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mcycleh, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mcycleh */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mcycleh, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mcycleh_ops */

        // ----------------------------------------------------------------
        // minstreth - MRW - Upper 32 bits of  minstret, RV32I only. 
        //
        /** Upper 32 bits of  minstret, RV32I only. assembler operations */
        struct minstreth_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read minstreth */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, minstreth" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write minstreth */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    minstreth, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to minstreth */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    minstreth, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to minstreth */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, minstreth, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to minstreth */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, minstreth, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for minstreth */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, minstreth, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for minstreth */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, minstreth, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for minstreth */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, minstreth, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for minstreth */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, minstreth, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for minstreth */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, minstreth, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for minstreth */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, minstreth, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for minstreth */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, minstreth, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for minstreth */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, minstreth, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* minstreth_ops */

        // ----------------------------------------------------------------
        // mhpmcounter3h - MRW - Upper 32 bits of  mhpmcounter3, RV32I only. 
        //
        /** Upper 32 bits of  mhpmcounter3, RV32I only. assembler operations */
        struct mhpmcounter3h_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter3h */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter3h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter3h */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mhpmcounter3h, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter3h */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mhpmcounter3h, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter3h */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter3h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter3h */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter3h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter3h */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter3h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter3h */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter3h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter3h */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter3h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter3h */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter3h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter3h */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter3h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter3h */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter3h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter3h */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter3h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter3h */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter3h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter3h_ops */

        // ----------------------------------------------------------------
        // mhpmcounter4h - MRW - Upper 32 bits of  mhpmcounter4, RV32I only. 
        //
        /** Upper 32 bits of  mhpmcounter4, RV32I only. assembler operations */
        struct mhpmcounter4h_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter4h */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter4h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter4h */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mhpmcounter4h, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter4h */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mhpmcounter4h, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter4h */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter4h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter4h */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter4h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter4h */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter4h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter4h */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter4h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter4h */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter4h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter4h */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter4h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter4h */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter4h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter4h */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter4h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter4h */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter4h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter4h */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter4h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter4h_ops */

        // ----------------------------------------------------------------
        // mhpmcounter31h - MRW - Upper 32 bits of  mhpmcounter31, RV32I only. 
        //
        /** Upper 32 bits of  mhpmcounter31, RV32I only. assembler operations */
        struct mhpmcounter31h_ops  {
            using datatype = uint_csr32_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmcounter31h */
            static uint32_t read(void) {
                uint_csr32_t value;        
                __asm__ volatile ("csrr    %0, mhpmcounter31h" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmcounter31h */
            static void write(uint_csr32_t value) {
                __asm__ volatile ("csrw    mhpmcounter31h, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmcounter31h */
            static void write_imm(uint_csr32_t value) {
                __asm__ volatile ("csrwi    mhpmcounter31h, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmcounter31h */
            static uint32_t read_write(uint_csr32_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmcounter31h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmcounter31h */
            static uint32_t read_write_imm(const uint8_t new_value) {
                uint_csr32_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmcounter31h, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmcounter31h */
            static void set_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmcounter31h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmcounter31h */
            static uint32_t read_set_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrs    %0, mhpmcounter31h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmcounter31h */
            static void clr_bits(uint_csr32_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmcounter31h, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmcounter31h */
            static uint32_t read_clr_bits(uint_csr32_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrc    %0, mhpmcounter31h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmcounter31h */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmcounter31h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmcounter31h */
            static uint32_t read_set_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrsi    %0, mhpmcounter31h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmcounter31h */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmcounter31h, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmcounter31h */
            static uint32_t read_clr_bits_imm(const uint8_t mask) {
                uint_csr32_t value;
                __asm__ volatile ("csrrci    %0, mhpmcounter31h, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmcounter31h_ops */

        // ----------------------------------------------------------------
        // mhpmevent4 - MRW - Machine performance-monitoring event selector. 
        //
        /** Machine performance-monitoring event selector. assembler operations */
        struct mhpmevent4_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmevent4 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhpmevent4" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmevent4 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mhpmevent4, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmevent4 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mhpmevent4, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmevent4 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmevent4, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmevent4 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmevent4, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmevent4 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmevent4, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmevent4 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mhpmevent4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmevent4 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmevent4, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmevent4 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mhpmevent4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmevent4 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmevent4, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmevent4 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mhpmevent4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmevent4 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmevent4, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmevent4 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mhpmevent4, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmevent4_ops */

        // ----------------------------------------------------------------
        // mhpmevent31 - MRW - Machine performance-monitoring event selector. 
        //
        /** Machine performance-monitoring event selector. assembler operations */
        struct mhpmevent31_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mhpmevent31 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mhpmevent31" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mhpmevent31 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mhpmevent31, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mhpmevent31 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mhpmevent31, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mhpmevent31 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mhpmevent31, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mhpmevent31 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mhpmevent31, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mhpmevent31 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mhpmevent31, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mhpmevent31 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mhpmevent31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mhpmevent31 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mhpmevent31, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mhpmevent31 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mhpmevent31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mhpmevent31 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mhpmevent31, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mhpmevent31 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mhpmevent31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mhpmevent31 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mhpmevent31, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mhpmevent31 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mhpmevent31, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mhpmevent31_ops */

        // ----------------------------------------------------------------
        // tselect - MRW - Debug/Trace trigger register select. 
        //
        /** Debug/Trace trigger register select. assembler operations */
        struct tselect_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read tselect */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, tselect" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write tselect */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    tselect, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to tselect */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    tselect, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to tselect */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, tselect, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to tselect */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, tselect, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for tselect */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, tselect, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for tselect */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, tselect, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for tselect */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, tselect, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for tselect */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, tselect, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for tselect */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, tselect, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for tselect */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, tselect, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for tselect */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, tselect, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for tselect */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, tselect, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* tselect_ops */

        // ----------------------------------------------------------------
        // tdata1 - MRW - First Debug/Trace trigger data register. 
        //
        /** First Debug/Trace trigger data register. assembler operations */
        struct tdata1_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read tdata1 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, tdata1" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write tdata1 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    tdata1, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to tdata1 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    tdata1, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to tdata1 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, tdata1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to tdata1 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, tdata1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for tdata1 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, tdata1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for tdata1 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, tdata1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for tdata1 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, tdata1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for tdata1 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, tdata1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for tdata1 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, tdata1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for tdata1 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, tdata1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for tdata1 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, tdata1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for tdata1 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, tdata1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* tdata1_ops */

        // ----------------------------------------------------------------
        // tdata2 - MRW - Second Debug/Trace trigger data register. 
        //
        /** Second Debug/Trace trigger data register. assembler operations */
        struct tdata2_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read tdata2 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, tdata2" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write tdata2 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    tdata2, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to tdata2 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    tdata2, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to tdata2 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, tdata2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to tdata2 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, tdata2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for tdata2 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, tdata2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for tdata2 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, tdata2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for tdata2 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, tdata2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for tdata2 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, tdata2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for tdata2 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, tdata2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for tdata2 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, tdata2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for tdata2 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, tdata2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for tdata2 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, tdata2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* tdata2_ops */

        // ----------------------------------------------------------------
        // tdata3 - MRW - Third Debug/Trace trigger data register. 
        //
        /** Third Debug/Trace trigger data register. assembler operations */
        struct tdata3_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read tdata3 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, tdata3" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write tdata3 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    tdata3, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to tdata3 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    tdata3, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to tdata3 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, tdata3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to tdata3 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, tdata3, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for tdata3 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, tdata3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for tdata3 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, tdata3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for tdata3 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, tdata3, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for tdata3 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, tdata3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for tdata3 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, tdata3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for tdata3 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, tdata3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for tdata3 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, tdata3, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for tdata3 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, tdata3, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* tdata3_ops */

        // ----------------------------------------------------------------
        // dcsr - DRW - Debug control and status register. 
        //
        /** Debug control and status register. assembler operations */
        struct dcsr_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = DRW; 
            
            /** Read dcsr */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, dcsr" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write dcsr */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    dcsr, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to dcsr */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    dcsr, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to dcsr */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, dcsr, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to dcsr */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, dcsr, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for dcsr */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, dcsr, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for dcsr */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, dcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for dcsr */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, dcsr, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for dcsr */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, dcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for dcsr */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, dcsr, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for dcsr */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, dcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for dcsr */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, dcsr, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for dcsr */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, dcsr, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* dcsr_ops */

        // ----------------------------------------------------------------
        // dpc - DRW - Debug PC. 
        //
        /** Debug PC. assembler operations */
        struct dpc_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = DRW; 
            
            /** Read dpc */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, dpc" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write dpc */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    dpc, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to dpc */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    dpc, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to dpc */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, dpc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to dpc */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, dpc, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for dpc */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, dpc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for dpc */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, dpc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for dpc */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, dpc, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for dpc */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, dpc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for dpc */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, dpc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for dpc */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, dpc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for dpc */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, dpc, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for dpc */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, dpc, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* dpc_ops */

        // ----------------------------------------------------------------
        // dscratch0 - DRW - Debug scratch register 0. 
        //
        /** Debug scratch register 0. assembler operations */
        struct dscratch0_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = DRW; 
            
            /** Read dscratch0 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, dscratch0" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write dscratch0 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    dscratch0, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to dscratch0 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    dscratch0, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to dscratch0 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, dscratch0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to dscratch0 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, dscratch0, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for dscratch0 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, dscratch0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for dscratch0 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, dscratch0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for dscratch0 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, dscratch0, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for dscratch0 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, dscratch0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for dscratch0 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, dscratch0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for dscratch0 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, dscratch0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for dscratch0 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, dscratch0, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for dscratch0 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, dscratch0, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* dscratch0_ops */

        // ----------------------------------------------------------------
        // dscratch1 - DRW - Debug scratch register 1. 
        //
        /** Debug scratch register 1. assembler operations */
        struct dscratch1_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = DRW; 
            
            /** Read dscratch1 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, dscratch1" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write dscratch1 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    dscratch1, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to dscratch1 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    dscratch1, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to dscratch1 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, dscratch1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to dscratch1 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, dscratch1, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for dscratch1 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, dscratch1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for dscratch1 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, dscratch1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for dscratch1 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, dscratch1, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for dscratch1 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, dscratch1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for dscratch1 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, dscratch1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for dscratch1 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, dscratch1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for dscratch1 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, dscratch1, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for dscratch1 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, dscratch1, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* dscratch1_ops */

        // ----------------------------------------------------------------
        // hie - HRW - Hypervisor interrupt-enable register. 
        //
        /** Hypervisor interrupt-enable register. assembler operations */
        struct hie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hie_ops */

        // ----------------------------------------------------------------
        // hgeie - HRW - Hypervisor guest external interrupt-enable register. 
        //
        /** Hypervisor guest external interrupt-enable register. assembler operations */
        struct hgeie_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hgeie */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hgeie" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hgeie */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hgeie, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hgeie */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hgeie, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hgeie */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hgeie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hgeie */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hgeie, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hgeie */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hgeie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hgeie */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hgeie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hgeie */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hgeie, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hgeie */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hgeie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hgeie */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hgeie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hgeie */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hgeie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hgeie */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hgeie, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hgeie */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hgeie, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hgeie_ops */

        // ----------------------------------------------------------------
        // htval - HRW - Hypervisor bad guest physical address. 
        //
        /** Hypervisor bad guest physical address. assembler operations */
        struct htval_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read htval */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, htval" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write htval */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    htval, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to htval */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    htval, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to htval */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, htval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to htval */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, htval, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for htval */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, htval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for htval */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, htval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for htval */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, htval, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for htval */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, htval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for htval */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, htval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for htval */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, htval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for htval */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, htval, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for htval */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, htval, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* htval_ops */

        // ----------------------------------------------------------------
        // hip - HRW - Hypervisor interrupt pending. 
        //
        /** Hypervisor interrupt pending. assembler operations */
        struct hip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read hip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write hip */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    hip, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to hip */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    hip, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to hip */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, hip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to hip */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, hip, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for hip */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, hip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for hip */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, hip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for hip */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, hip, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for hip */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, hip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for hip */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, hip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for hip */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, hip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for hip */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, hip, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for hip */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, hip, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* hip_ops */

        // ----------------------------------------------------------------
        // htinst - HRW - Hypervisor trap instruction (transformed). 
        //
        /** Hypervisor trap instruction (transformed). assembler operations */
        struct htinst_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRW; 
            
            /** Read htinst */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, htinst" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write htinst */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    htinst, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to htinst */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    htinst, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to htinst */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, htinst, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to htinst */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, htinst, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for htinst */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, htinst, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for htinst */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, htinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for htinst */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, htinst, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for htinst */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, htinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for htinst */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, htinst, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for htinst */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, htinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for htinst */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, htinst, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for htinst */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, htinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* htinst_ops */

        // ----------------------------------------------------------------
        // hgeip - HRO - Hypervisor guest external interrupt pending. 
        //
        /** Hypervisor guest external interrupt pending. assembler operations */
        struct hgeip_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = HRO; 
            
            /** Read hgeip */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, hgeip" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
        }; /* hgeip_ops */

        // ----------------------------------------------------------------
        // mtinst - MRW - Machine trap instruction (transformed). 
        //
        /** Machine trap instruction (transformed). assembler operations */
        struct mtinst_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mtinst */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mtinst" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mtinst */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mtinst, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mtinst */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mtinst, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mtinst */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mtinst, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mtinst */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mtinst, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mtinst */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mtinst, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mtinst */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mtinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mtinst */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mtinst, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mtinst */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mtinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mtinst */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mtinst, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mtinst */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mtinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mtinst */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mtinst, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mtinst */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mtinst, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mtinst_ops */

        // ----------------------------------------------------------------
        // mtval2 - MRW - Machine bad guest physical address. 
        //
        /** Machine bad guest physical address. assembler operations */
        struct mtval2_ops  {
            using datatype = uint_xlen_t;
            static constexpr priv_t priv = MRW; 
            
            /** Read mtval2 */
            static uint_xlen_t read(void) {
                uint_xlen_t value;        
                __asm__ volatile ("csrr    %0, mtval2" 
                                  : "=r" (value)  /* output : register */
                                  : /* input : none */
                                  : /* clobbers: none */);
                return value;
            }
            
            
            /** Write mtval2 */
            static void write(uint_xlen_t value) {
                __asm__ volatile ("csrw    mtval2, %0" 
                                  : /* output: none */ 
                                  : "r" (value) /* input : from register */
                                  : /* clobbers: none */);
            }
            /** Write immediate value to mtval2 */
            static void write_imm(uint_xlen_t value) {
                __asm__ volatile ("csrwi    mtval2, %0" 
                                  : /* output: none */ 
                                  : "i" (value) /* input : from immediate */
                                  : /* clobbers: none */);
            }
            /** Read and then write to mtval2 */
            static uint_xlen_t read_write(uint_xlen_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrw    %0, mtval2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "r" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
            /** Read and then write immediate value to mtval2 */
            static uint_xlen_t read_write_imm(const uint8_t new_value) {
                uint_xlen_t prev_value;
                __asm__ volatile ("csrrwi    %0, mtval2, %1"  
                                  : "=r" (prev_value) /* output: register %0 */
                                  : "i" (new_value)  /* input : register */
                                  : /* clobbers: none */);
                return prev_value;
            }
        
            // ------------------------------------------
            // Register CSR bit set and clear instructions

            /** Atomic modify and set bits for mtval2 */
            static void set_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrs    zero, mtval2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits for mtval2 */
            static uint32_t read_set_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrs    %0, mtval2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits for mtval2 */
            static void clr_bits(uint_xlen_t mask) {
                __asm__ volatile ("csrrc    zero, mtval2, %0"  
                                  : /* output: none */ 
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits for mtval2 */
            static uint32_t read_clr_bits(uint_xlen_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrc    %0, mtval2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "r" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
        
            // ------------------------------------------
            // Immediate value CSR bit set and clear instructions (only up to 5 bits)
        
            /** Atomic modify and set bits from immediate for mtval2 */
            static void set_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrsi    zero, mtval2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and set bits from immediate for mtval2 */
            static uint_xlen_t read_set_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrsi    %0, mtval2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            /** Atomic modify and clear bits from immediate for mtval2 */
            static void clr_bits_imm(const uint8_t mask) {
                __asm__ volatile ("csrrci    zero, mtval2, %0"  
                                  : /* output: none */ 
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
            }
            /** Atomic read and then and clear bits from immediate for mtval2 */
            static uint_xlen_t read_clr_bits_imm(const uint8_t mask) {
                uint_xlen_t value;
                __asm__ volatile ("csrrci    %0, mtval2, %1"  
                                  : "=r" (value) /* output: register %0 */
                                  : "i" (mask)  /* input : register */
                                  : /* clobbers: none */);
                return value;
            }
            
        }; /* mtval2_ops */
    } /* csr */
} /* riscv */


// ------------------------------------------------------------------------
// Register and field interface classes.
namespace riscv {
    namespace csr {
        /* Machine ISA */
        template<class OPS> class misa_reg : public read_write_reg<OPS>
        {
        };
        using misa = misa_reg<riscv::csr::misa_ops>;
        /* Machine Vendor ID */
        template<class OPS> class mvendorid_reg : public read_only_reg<OPS>
        {
        };
        using mvendorid = mvendorid_reg<riscv::csr::mvendorid_ops>;
        /* Machine Architecture ID */
        template<class OPS> class marchid_reg : public read_only_reg<OPS>
        {
        };
        using marchid = marchid_reg<riscv::csr::marchid_ops>;
        /* Machine Implementation ID */
        template<class OPS> class mimpid_reg : public read_only_reg<OPS>
        {
        };
        using mimpid = mimpid_reg<riscv::csr::mimpid_ops>;
        /* Hardware Thread ID */
        template<class OPS> class mhartid_reg : public read_only_reg<OPS>
        {
        };
        using mhartid = mhartid_reg<riscv::csr::mhartid_ops>;
        /* Machine Status */
        template<class OPS> class mstatus_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mstatus_data::mie> mie;
                read_write_field<OPS, riscv::csr::mstatus_data::sie> sie;
                read_write_field<OPS, riscv::csr::mstatus_data::mpie> mpie;
                read_write_field<OPS, riscv::csr::mstatus_data::spie> spie;
                read_write_field<OPS, riscv::csr::mstatus_data::mprv> mprv;
                read_write_field<OPS, riscv::csr::mstatus_data::mpp> mpp;
                read_write_field<OPS, riscv::csr::mstatus_data::spp> spp;
        };
        using mstatus = mstatus_reg<riscv::csr::mstatus_ops>;
        /* Additional machine status register, RV32 only. */
        template<class OPS> class mstatush_reg : public read_write_reg<OPS>
        {
        };
        using mstatush = mstatush_reg<riscv::csr::mstatush_ops>;
        /* Machine Trap Vector Base Address */
        template<class OPS> class mtvec_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mtvec_data::base> base;
                read_write_field<OPS, riscv::csr::mtvec_data::mode> mode;
        };
        using mtvec = mtvec_reg<riscv::csr::mtvec_ops>;
        /* Machine Exception Delegation */
        template<class OPS> class medeleg_reg : public read_write_reg<OPS>
        {
        };
        using medeleg = medeleg_reg<riscv::csr::medeleg_ops>;
        /* Machine Interrupt Delegation */
        template<class OPS> class mideleg_reg : public read_write_reg<OPS>
        {
        };
        using mideleg = mideleg_reg<riscv::csr::mideleg_ops>;
        /* Machine Interrupt Pending */
        template<class OPS> class mip_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mip_data::msi> msi;
                read_write_field<OPS, riscv::csr::mip_data::mti> mti;
                read_write_field<OPS, riscv::csr::mip_data::mei> mei;
                read_write_field<OPS, riscv::csr::mip_data::ssi> ssi;
                read_write_field<OPS, riscv::csr::mip_data::sti> sti;
                read_write_field<OPS, riscv::csr::mip_data::sei> sei;
                read_write_field<OPS, riscv::csr::mip_data::usi> usi;
                read_write_field<OPS, riscv::csr::mip_data::uti> uti;
                read_write_field<OPS, riscv::csr::mip_data::uei> uei;
                read_write_field<OPS, riscv::csr::mip_data::platform_defined> platform_defined;
        };
        using mip = mip_reg<riscv::csr::mip_ops>;
        /* Machine Interrupt Enable */
        template<class OPS> class mie_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mie_data::msi> msi;
                read_write_field<OPS, riscv::csr::mie_data::mti> mti;
                read_write_field<OPS, riscv::csr::mie_data::mei> mei;
                read_write_field<OPS, riscv::csr::mie_data::ssi> ssi;
                read_write_field<OPS, riscv::csr::mie_data::sti> sti;
                read_write_field<OPS, riscv::csr::mie_data::sei> sei;
                read_write_field<OPS, riscv::csr::mie_data::usi> usi;
                read_write_field<OPS, riscv::csr::mie_data::uti> uti;
                read_write_field<OPS, riscv::csr::mie_data::uei> uei;
                read_write_field<OPS, riscv::csr::mie_data::platform_defined> platform_defined;
        };
        using mie = mie_reg<riscv::csr::mie_ops>;
        /* Machine Counter Inhibit */
        template<class OPS> class mcountinhibit_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mcountinhibit_data::cy> cy;
                read_write_field<OPS, riscv::csr::mcountinhibit_data::ir> ir;
                read_write_field<OPS, riscv::csr::mcountinhibit_data::hpm> hpm;
        };
        using mcountinhibit = mcountinhibit_reg<riscv::csr::mcountinhibit_ops>;
        /* Clock Cycles Executed Counter */
        template<class OPS> class mcycle_reg : public read_write_reg<OPS>
        {
        };
        using mcycle = mcycle_reg<riscv::csr::mcycle_ops>;
        /* Number of Instructions Retired Counter */
        template<class OPS> class minstret_reg : public read_write_reg<OPS>
        {
        };
        using minstret = minstret_reg<riscv::csr::minstret_ops>;
        /* Event Counters */
        template<class OPS> class mhpmcounter3_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter3 = mhpmcounter3_reg<riscv::csr::mhpmcounter3_ops>;
        /* Event Counter Event Select */
        template<class OPS> class mhpmevent3_reg : public read_write_reg<OPS>
        {
        };
        using mhpmevent3 = mhpmevent3_reg<riscv::csr::mhpmevent3_ops>;
        /* Counter Enable */
        template<class OPS> class mcounteren_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mcounteren_data::cy> cy;
                read_write_field<OPS, riscv::csr::mcounteren_data::tm> tm;
                read_write_field<OPS, riscv::csr::mcounteren_data::ir> ir;
                read_write_field<OPS, riscv::csr::mcounteren_data::hpm> hpm;
        };
        using mcounteren = mcounteren_reg<riscv::csr::mcounteren_ops>;
        /* Counter Enable */
        template<class OPS> class scounteren_reg : public read_write_reg<OPS>
        {
        };
        using scounteren = scounteren_reg<riscv::csr::scounteren_ops>;
        /* Machine Mode Scratch Register */
        template<class OPS> class mscratch_reg : public read_write_reg<OPS>
        {
        };
        using mscratch = mscratch_reg<riscv::csr::mscratch_ops>;
        /* Machine Exception Program Counter */
        template<class OPS> class mepc_reg : public read_write_reg<OPS>
        {
        };
        using mepc = mepc_reg<riscv::csr::mepc_ops>;
        /* Machine Exception Cause */
        template<class OPS> class mcause_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::mcause_data::interrupt> interrupt;
                read_write_field<OPS, riscv::csr::mcause_data::exception_code> exception_code;
        };
        using mcause = mcause_reg<riscv::csr::mcause_ops>;
        /* Machine Trap Value */
        template<class OPS> class mtval_reg : public read_write_reg<OPS>
        {
        };
        using mtval = mtval_reg<riscv::csr::mtval_ops>;
        /* Supervisor Mode Scratch Register */
        template<class OPS> class sscratch_reg : public read_write_reg<OPS>
        {
        };
        using sscratch = sscratch_reg<riscv::csr::sscratch_ops>;
        /* Supervisor Exception Program Counter */
        template<class OPS> class sepc_reg : public read_write_reg<OPS>
        {
        };
        using sepc = sepc_reg<riscv::csr::sepc_ops>;
        /* Supervisor Exception Cause */
        template<class OPS> class scause_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::scause_data::interrupt> interrupt;
                read_write_field<OPS, riscv::csr::scause_data::exception_code> exception_code;
        };
        using scause = scause_reg<riscv::csr::scause_ops>;
        /* Supervisor Status */
        template<class OPS> class sstatus_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::sstatus_data::sie> sie;
                read_write_field<OPS, riscv::csr::sstatus_data::spie> spie;
                read_write_field<OPS, riscv::csr::sstatus_data::spp> spp;
        };
        using sstatus = sstatus_reg<riscv::csr::sstatus_ops>;
        /* Supervisor Trap Vector Base Address */
        template<class OPS> class stvec_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::stvec_data::base> base;
                read_write_field<OPS, riscv::csr::stvec_data::mode> mode;
        };
        using stvec = stvec_reg<riscv::csr::stvec_ops>;
        /* Supervisor Interrupt Delegation */
        template<class OPS> class sideleg_reg : public read_write_reg<OPS>
        {
        };
        using sideleg = sideleg_reg<riscv::csr::sideleg_ops>;
        /* Supervisor Exception Delegation */
        template<class OPS> class sedeleg_reg : public read_write_reg<OPS>
        {
        };
        using sedeleg = sedeleg_reg<riscv::csr::sedeleg_ops>;
        /* Supervisor Interrupt Pending */
        template<class OPS> class sip_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::sip_data::ssi> ssi;
                read_write_field<OPS, riscv::csr::sip_data::sti> sti;
                read_write_field<OPS, riscv::csr::sip_data::sei> sei;
                read_write_field<OPS, riscv::csr::sip_data::usi> usi;
                read_write_field<OPS, riscv::csr::sip_data::uti> uti;
                read_write_field<OPS, riscv::csr::sip_data::uei> uei;
        };
        using sip = sip_reg<riscv::csr::sip_ops>;
        /* Supervisor Interrupt Enable */
        template<class OPS> class sie_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::sie_data::ssi> ssi;
                read_write_field<OPS, riscv::csr::sie_data::sti> sti;
                read_write_field<OPS, riscv::csr::sie_data::sei> sei;
                read_write_field<OPS, riscv::csr::sie_data::usi> usi;
                read_write_field<OPS, riscv::csr::sie_data::uti> uti;
                read_write_field<OPS, riscv::csr::sie_data::uei> uei;
        };
        using sie = sie_reg<riscv::csr::sie_ops>;
        /* User mode restricted view of mstatus */
        template<class OPS> class ustatus_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::ustatus_data::uie> uie;
                read_write_field<OPS, riscv::csr::ustatus_data::upie> upie;
        };
        using ustatus = ustatus_reg<riscv::csr::ustatus_ops>;
        /* User Interrupt Pending */
        template<class OPS> class uip_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::uip_data::usi> usi;
                read_write_field<OPS, riscv::csr::uip_data::uti> uti;
                read_write_field<OPS, riscv::csr::uip_data::uei> uei;
        };
        using uip = uip_reg<riscv::csr::uip_ops>;
        /* User Interrupt Enable */
        template<class OPS> class uie_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::uie_data::usi> usi;
                read_write_field<OPS, riscv::csr::uie_data::uti> uti;
                read_write_field<OPS, riscv::csr::uie_data::uei> uei;
        };
        using uie = uie_reg<riscv::csr::uie_ops>;
        /* User Mode Scratch Register */
        template<class OPS> class uscratch_reg : public read_write_reg<OPS>
        {
        };
        using uscratch = uscratch_reg<riscv::csr::uscratch_ops>;
        /* User Exception Program Counter */
        template<class OPS> class uepc_reg : public read_write_reg<OPS>
        {
        };
        using uepc = uepc_reg<riscv::csr::uepc_ops>;
        /* User Exception Cause */
        template<class OPS> class ucause_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::ucause_data::interrupt> interrupt;
                read_write_field<OPS, riscv::csr::ucause_data::exception_code> exception_code;
        };
        using ucause = ucause_reg<riscv::csr::ucause_ops>;
        /* User Trap Vector Base Address */
        template<class OPS> class utvec_reg : public read_write_reg<OPS>
        {
            public:
                read_write_field<OPS, riscv::csr::utvec_data::base> base;
                read_write_field<OPS, riscv::csr::utvec_data::mode> mode;
        };
        using utvec = utvec_reg<riscv::csr::utvec_ops>;
        /* User Trap Value */
        template<class OPS> class utval_reg : public read_write_reg<OPS>
        {
        };
        using utval = utval_reg<riscv::csr::utval_ops>;
        /* Floating-Point Accrued Exceptions. */
        template<class OPS> class fflags_reg : public read_write_reg<OPS>
        {
        };
        using fflags = fflags_reg<riscv::csr::fflags_ops>;
        /* Floating-Point Dynamic Rounding Mode. */
        template<class OPS> class frm_reg : public read_write_reg<OPS>
        {
        };
        using frm = frm_reg<riscv::csr::frm_ops>;
        /* Floating-Point Control and Status */
        template<class OPS> class fcsr_reg : public read_write_reg<OPS>
        {
        };
        using fcsr = fcsr_reg<riscv::csr::fcsr_ops>;
        /* Cycle counter for RDCYCLE instruction. */
        template<class OPS> class cycle_reg : public read_only_reg<OPS>
        {
        };
        using cycle = cycle_reg<riscv::csr::cycle_ops>;
        /* Timer for RDTIME instruction. */
        template<class OPS> class time_reg : public read_only_reg<OPS>
        {
        };
        using time = time_reg<riscv::csr::time_ops>;
        /* Instructions-retired counter for RDINSTRET instruction. */
        template<class OPS> class instret_reg : public read_only_reg<OPS>
        {
        };
        using instret = instret_reg<riscv::csr::instret_ops>;
        /* Performance-monitoring counter. */
        template<class OPS> class hpmcounter3_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter3 = hpmcounter3_reg<riscv::csr::hpmcounter3_ops>;
        /* Performance-monitoring counter. */
        template<class OPS> class hpmcounter4_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter4 = hpmcounter4_reg<riscv::csr::hpmcounter4_ops>;
        /* Performance-monitoring counter. */
        template<class OPS> class hpmcounter31_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter31 = hpmcounter31_reg<riscv::csr::hpmcounter31_ops>;
        /* Upper 32 bits of  cycle, RV32I only. */
        template<class OPS> class cycleh_reg : public read_only_reg<OPS>
        {
        };
        using cycleh = cycleh_reg<riscv::csr::cycleh_ops>;
        /* Upper 32 bits of  time, RV32I only. */
        template<class OPS> class timeh_reg : public read_only_reg<OPS>
        {
        };
        using timeh = timeh_reg<riscv::csr::timeh_ops>;
        /* Upper 32 bits of  instret, RV32I only. */
        template<class OPS> class instreth_reg : public read_only_reg<OPS>
        {
        };
        using instreth = instreth_reg<riscv::csr::instreth_ops>;
        /* Upper 32 bits of  hpmcounter3, RV32I only. */
        template<class OPS> class hpmcounter3h_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter3h = hpmcounter3h_reg<riscv::csr::hpmcounter3h_ops>;
        /* Upper 32 bits of  hpmcounter4, RV32I only. */
        template<class OPS> class hpmcounter4h_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter4h = hpmcounter4h_reg<riscv::csr::hpmcounter4h_ops>;
        /* Upper 32 bits of  hpmcounter31, RV32I only. */
        template<class OPS> class hpmcounter31h_reg : public read_only_reg<OPS>
        {
        };
        using hpmcounter31h = hpmcounter31h_reg<riscv::csr::hpmcounter31h_ops>;
        /* Supervisor bad address or instruction. */
        template<class OPS> class stval_reg : public read_write_reg<OPS>
        {
        };
        using stval = stval_reg<riscv::csr::stval_ops>;
        /* Supervisor address translation and protection. */
        template<class OPS> class satp_reg : public read_write_reg<OPS>
        {
        };
        using satp = satp_reg<riscv::csr::satp_ops>;
        /* Hypervisor status register. */
        template<class OPS> class hstatus_reg : public read_write_reg<OPS>
        {
        };
        using hstatus = hstatus_reg<riscv::csr::hstatus_ops>;
        /* Hypervisor exception delegation register. */
        template<class OPS> class hedeleg_reg : public read_write_reg<OPS>
        {
        };
        using hedeleg = hedeleg_reg<riscv::csr::hedeleg_ops>;
        /* Hypervisor interrupt delegation register. */
        template<class OPS> class hideleg_reg : public read_write_reg<OPS>
        {
        };
        using hideleg = hideleg_reg<riscv::csr::hideleg_ops>;
        /* Hypervisor counter enable. */
        template<class OPS> class hcounteren_reg : public read_write_reg<OPS>
        {
        };
        using hcounteren = hcounteren_reg<riscv::csr::hcounteren_ops>;
        /* Hypervisor guest address translation and protection. */
        template<class OPS> class hgatp_reg : public read_write_reg<OPS>
        {
        };
        using hgatp = hgatp_reg<riscv::csr::hgatp_ops>;
        /* Delta for VS/VU-mode timer. */
        template<class OPS> class htimedelta_reg : public read_write_reg<OPS>
        {
        };
        using htimedelta = htimedelta_reg<riscv::csr::htimedelta_ops>;
        /* Upper 32 bits of  htimedelta, RV32I only. */
        template<class OPS> class htimedeltah_reg : public read_write_reg<OPS>
        {
        };
        using htimedeltah = htimedeltah_reg<riscv::csr::htimedeltah_ops>;
        /* Virtual supervisor status register. */
        template<class OPS> class vsstatus_reg : public read_write_reg<OPS>
        {
        };
        using vsstatus = vsstatus_reg<riscv::csr::vsstatus_ops>;
        /* Virtual supervisor interrupt-enable register. */
        template<class OPS> class vsie_reg : public read_write_reg<OPS>
        {
        };
        using vsie = vsie_reg<riscv::csr::vsie_ops>;
        /* Virtual supervisor trap handler base address. */
        template<class OPS> class vstvec_reg : public read_write_reg<OPS>
        {
        };
        using vstvec = vstvec_reg<riscv::csr::vstvec_ops>;
        /* Virtual supervisor scratch register. */
        template<class OPS> class vsscratch_reg : public read_write_reg<OPS>
        {
        };
        using vsscratch = vsscratch_reg<riscv::csr::vsscratch_ops>;
        /* Virtual supervisor exception program counter. */
        template<class OPS> class vsepc_reg : public read_write_reg<OPS>
        {
        };
        using vsepc = vsepc_reg<riscv::csr::vsepc_ops>;
        /* Virtual supervisor trap cause. */
        template<class OPS> class vscause_reg : public read_write_reg<OPS>
        {
        };
        using vscause = vscause_reg<riscv::csr::vscause_ops>;
        /* Virtual supervisor bad address or instruction. */
        template<class OPS> class vstval_reg : public read_write_reg<OPS>
        {
        };
        using vstval = vstval_reg<riscv::csr::vstval_ops>;
        /* Virtual supervisor interrupt pending. */
        template<class OPS> class vsip_reg : public read_write_reg<OPS>
        {
        };
        using vsip = vsip_reg<riscv::csr::vsip_ops>;
        /* Virtual supervisor address translation and protection. */
        template<class OPS> class vsatp_reg : public read_write_reg<OPS>
        {
        };
        using vsatp = vsatp_reg<riscv::csr::vsatp_ops>;
        /* Base register. */
        template<class OPS> class mbase_reg : public read_write_reg<OPS>
        {
        };
        using mbase = mbase_reg<riscv::csr::mbase_ops>;
        /* Bound register. */
        template<class OPS> class mbound_reg : public read_write_reg<OPS>
        {
        };
        using mbound = mbound_reg<riscv::csr::mbound_ops>;
        /* Instruction base register. */
        template<class OPS> class mibase_reg : public read_write_reg<OPS>
        {
        };
        using mibase = mibase_reg<riscv::csr::mibase_ops>;
        /* Instruction bound register. */
        template<class OPS> class mibound_reg : public read_write_reg<OPS>
        {
        };
        using mibound = mibound_reg<riscv::csr::mibound_ops>;
        /* Data base register. */
        template<class OPS> class mdbase_reg : public read_write_reg<OPS>
        {
        };
        using mdbase = mdbase_reg<riscv::csr::mdbase_ops>;
        /* Data bound register. */
        template<class OPS> class mdbound_reg : public read_write_reg<OPS>
        {
        };
        using mdbound = mdbound_reg<riscv::csr::mdbound_ops>;
        /* Physical memory protection configuration. */
        template<class OPS> class pmpcfg0_reg : public read_write_reg<OPS>
        {
        };
        using pmpcfg0 = pmpcfg0_reg<riscv::csr::pmpcfg0_ops>;
        /* Physical memory protection configuration, RV32 only. */
        template<class OPS> class pmpcfg1_reg : public read_write_reg<OPS>
        {
        };
        using pmpcfg1 = pmpcfg1_reg<riscv::csr::pmpcfg1_ops>;
        /* Physical memory protection configuration. */
        template<class OPS> class pmpcfg2_reg : public read_write_reg<OPS>
        {
        };
        using pmpcfg2 = pmpcfg2_reg<riscv::csr::pmpcfg2_ops>;
        /* Physical memory protection configuration, RV32 only. */
        template<class OPS> class pmpcfg3_reg : public read_write_reg<OPS>
        {
        };
        using pmpcfg3 = pmpcfg3_reg<riscv::csr::pmpcfg3_ops>;
        /* Physical memory protection address register. */
        template<class OPS> class pmpaddr0_reg : public read_write_reg<OPS>
        {
        };
        using pmpaddr0 = pmpaddr0_reg<riscv::csr::pmpaddr0_ops>;
        /* Physical memory protection address register. */
        template<class OPS> class pmpaddr1_reg : public read_write_reg<OPS>
        {
        };
        using pmpaddr1 = pmpaddr1_reg<riscv::csr::pmpaddr1_ops>;
        /* Physical memory protection address register. */
        template<class OPS> class pmpaddr15_reg : public read_write_reg<OPS>
        {
        };
        using pmpaddr15 = pmpaddr15_reg<riscv::csr::pmpaddr15_ops>;
        /* Machine performance-monitoring counter. */
        template<class OPS> class mhpmcounter4_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter4 = mhpmcounter4_reg<riscv::csr::mhpmcounter4_ops>;
        /* Machine performance-monitoring counter. */
        template<class OPS> class mhpmcounter31_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter31 = mhpmcounter31_reg<riscv::csr::mhpmcounter31_ops>;
        /* Upper 32 bits of  mcycle, RV32I only. */
        template<class OPS> class mcycleh_reg : public read_write_reg<OPS>
        {
        };
        using mcycleh = mcycleh_reg<riscv::csr::mcycleh_ops>;
        /* Upper 32 bits of  minstret, RV32I only. */
        template<class OPS> class minstreth_reg : public read_write_reg<OPS>
        {
        };
        using minstreth = minstreth_reg<riscv::csr::minstreth_ops>;
        /* Upper 32 bits of  mhpmcounter3, RV32I only. */
        template<class OPS> class mhpmcounter3h_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter3h = mhpmcounter3h_reg<riscv::csr::mhpmcounter3h_ops>;
        /* Upper 32 bits of  mhpmcounter4, RV32I only. */
        template<class OPS> class mhpmcounter4h_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter4h = mhpmcounter4h_reg<riscv::csr::mhpmcounter4h_ops>;
        /* Upper 32 bits of  mhpmcounter31, RV32I only. */
        template<class OPS> class mhpmcounter31h_reg : public read_write_reg<OPS>
        {
        };
        using mhpmcounter31h = mhpmcounter31h_reg<riscv::csr::mhpmcounter31h_ops>;
        /* Machine performance-monitoring event selector. */
        template<class OPS> class mhpmevent4_reg : public read_write_reg<OPS>
        {
        };
        using mhpmevent4 = mhpmevent4_reg<riscv::csr::mhpmevent4_ops>;
        /* Machine performance-monitoring event selector. */
        template<class OPS> class mhpmevent31_reg : public read_write_reg<OPS>
        {
        };
        using mhpmevent31 = mhpmevent31_reg<riscv::csr::mhpmevent31_ops>;
        /* Debug/Trace trigger register select. */
        template<class OPS> class tselect_reg : public read_write_reg<OPS>
        {
        };
        using tselect = tselect_reg<riscv::csr::tselect_ops>;
        /* First Debug/Trace trigger data register. */
        template<class OPS> class tdata1_reg : public read_write_reg<OPS>
        {
        };
        using tdata1 = tdata1_reg<riscv::csr::tdata1_ops>;
        /* Second Debug/Trace trigger data register. */
        template<class OPS> class tdata2_reg : public read_write_reg<OPS>
        {
        };
        using tdata2 = tdata2_reg<riscv::csr::tdata2_ops>;
        /* Third Debug/Trace trigger data register. */
        template<class OPS> class tdata3_reg : public read_write_reg<OPS>
        {
        };
        using tdata3 = tdata3_reg<riscv::csr::tdata3_ops>;
        /* Debug control and status register. */
        template<class OPS> class dcsr_reg : public read_write_reg<OPS>
        {
        };
        using dcsr = dcsr_reg<riscv::csr::dcsr_ops>;
        /* Debug PC. */
        template<class OPS> class dpc_reg : public read_write_reg<OPS>
        {
        };
        using dpc = dpc_reg<riscv::csr::dpc_ops>;
        /* Debug scratch register 0. */
        template<class OPS> class dscratch0_reg : public read_write_reg<OPS>
        {
        };
        using dscratch0 = dscratch0_reg<riscv::csr::dscratch0_ops>;
        /* Debug scratch register 1. */
        template<class OPS> class dscratch1_reg : public read_write_reg<OPS>
        {
        };
        using dscratch1 = dscratch1_reg<riscv::csr::dscratch1_ops>;
        /* Hypervisor interrupt-enable register. */
        template<class OPS> class hie_reg : public read_write_reg<OPS>
        {
        };
        using hie = hie_reg<riscv::csr::hie_ops>;
        /* Hypervisor guest external interrupt-enable register. */
        template<class OPS> class hgeie_reg : public read_write_reg<OPS>
        {
        };
        using hgeie = hgeie_reg<riscv::csr::hgeie_ops>;
        /* Hypervisor bad guest physical address. */
        template<class OPS> class htval_reg : public read_write_reg<OPS>
        {
        };
        using htval = htval_reg<riscv::csr::htval_ops>;
        /* Hypervisor interrupt pending. */
        template<class OPS> class hip_reg : public read_write_reg<OPS>
        {
        };
        using hip = hip_reg<riscv::csr::hip_ops>;
        /* Hypervisor trap instruction (transformed). */
        template<class OPS> class htinst_reg : public read_write_reg<OPS>
        {
        };
        using htinst = htinst_reg<riscv::csr::htinst_ops>;
        /* Hypervisor guest external interrupt pending. */
        template<class OPS> class hgeip_reg : public read_only_reg<OPS>
        {
        };
        using hgeip = hgeip_reg<riscv::csr::hgeip_ops>;
        /* Machine trap instruction (transformed). */
        template<class OPS> class mtinst_reg : public read_write_reg<OPS>
        {
        };
        using mtinst = mtinst_reg<riscv::csr::mtinst_ops>;
        /* Machine bad guest physical address. */
        template<class OPS> class mtval2_reg : public read_write_reg<OPS>
        {
        };
        using mtval2 = mtval2_reg<riscv::csr::mtval2_ops>;

        /** Encapsulate all CSRs in a single structure.
           - No storage is required by this class.
         */
        struct all {
            /* Machine ISA */
            riscv::csr::misa misa;
            /* Machine Vendor ID */
            riscv::csr::mvendorid mvendorid;
            /* Machine Architecture ID */
            riscv::csr::marchid marchid;
            /* Machine Implementation ID */
            riscv::csr::mimpid mimpid;
            /* Hardware Thread ID */
            riscv::csr::mhartid mhartid;
            /* Machine Status */
            riscv::csr::mstatus mstatus;
            /* Additional machine status register, RV32 only. */
            riscv::csr::mstatush mstatush;
            /* Machine Trap Vector Base Address */
            riscv::csr::mtvec mtvec;
            /* Machine Exception Delegation */
            riscv::csr::medeleg medeleg;
            /* Machine Interrupt Delegation */
            riscv::csr::mideleg mideleg;
            /* Machine Interrupt Pending */
            riscv::csr::mip mip;
            /* Machine Interrupt Enable */
            riscv::csr::mie mie;
            /* Machine Counter Inhibit */
            riscv::csr::mcountinhibit mcountinhibit;
            /* Clock Cycles Executed Counter */
            riscv::csr::mcycle mcycle;
            /* Number of Instructions Retired Counter */
            riscv::csr::minstret minstret;
            /* Event Counters */
            riscv::csr::mhpmcounter3 mhpmcounter3;
            /* Event Counter Event Select */
            riscv::csr::mhpmevent3 mhpmevent3;
            /* Counter Enable */
            riscv::csr::mcounteren mcounteren;
            /* Counter Enable */
            riscv::csr::scounteren scounteren;
            /* Machine Mode Scratch Register */
            riscv::csr::mscratch mscratch;
            /* Machine Exception Program Counter */
            riscv::csr::mepc mepc;
            /* Machine Exception Cause */
            riscv::csr::mcause mcause;
            /* Machine Trap Value */
            riscv::csr::mtval mtval;
            /* Supervisor Mode Scratch Register */
            riscv::csr::sscratch sscratch;
            /* Supervisor Exception Program Counter */
            riscv::csr::sepc sepc;
            /* Supervisor Exception Cause */
            riscv::csr::scause scause;
            /* Supervisor Status */
            riscv::csr::sstatus sstatus;
            /* Supervisor Trap Vector Base Address */
            riscv::csr::stvec stvec;
            /* Supervisor Interrupt Delegation */
            riscv::csr::sideleg sideleg;
            /* Supervisor Exception Delegation */
            riscv::csr::sedeleg sedeleg;
            /* Supervisor Interrupt Pending */
            riscv::csr::sip sip;
            /* Supervisor Interrupt Enable */
            riscv::csr::sie sie;
            /* User mode restricted view of mstatus */
            riscv::csr::ustatus ustatus;
            /* User Interrupt Pending */
            riscv::csr::uip uip;
            /* User Interrupt Enable */
            riscv::csr::uie uie;
            /* User Mode Scratch Register */
            riscv::csr::uscratch uscratch;
            /* User Exception Program Counter */
            riscv::csr::uepc uepc;
            /* User Exception Cause */
            riscv::csr::ucause ucause;
            /* User Trap Vector Base Address */
            riscv::csr::utvec utvec;
            /* User Trap Value */
            riscv::csr::utval utval;
            /* Floating-Point Accrued Exceptions. */
            riscv::csr::fflags fflags;
            /* Floating-Point Dynamic Rounding Mode. */
            riscv::csr::frm frm;
            /* Floating-Point Control and Status */
            riscv::csr::fcsr fcsr;
            /* Cycle counter for RDCYCLE instruction. */
            riscv::csr::cycle cycle;
            /* Timer for RDTIME instruction. */
            riscv::csr::time time;
            /* Instructions-retired counter for RDINSTRET instruction. */
            riscv::csr::instret instret;
            /* Performance-monitoring counter. */
            riscv::csr::hpmcounter3 hpmcounter3;
            /* Performance-monitoring counter. */
            riscv::csr::hpmcounter4 hpmcounter4;
            /* Performance-monitoring counter. */
            riscv::csr::hpmcounter31 hpmcounter31;
            /* Upper 32 bits of  cycle, RV32I only. */
            riscv::csr::cycleh cycleh;
            /* Upper 32 bits of  time, RV32I only. */
            riscv::csr::timeh timeh;
            /* Upper 32 bits of  instret, RV32I only. */
            riscv::csr::instreth instreth;
            /* Upper 32 bits of  hpmcounter3, RV32I only. */
            riscv::csr::hpmcounter3h hpmcounter3h;
            /* Upper 32 bits of  hpmcounter4, RV32I only. */
            riscv::csr::hpmcounter4h hpmcounter4h;
            /* Upper 32 bits of  hpmcounter31, RV32I only. */
            riscv::csr::hpmcounter31h hpmcounter31h;
            /* Supervisor bad address or instruction. */
            riscv::csr::stval stval;
            /* Supervisor address translation and protection. */
            riscv::csr::satp satp;
            /* Hypervisor status register. */
            riscv::csr::hstatus hstatus;
            /* Hypervisor exception delegation register. */
            riscv::csr::hedeleg hedeleg;
            /* Hypervisor interrupt delegation register. */
            riscv::csr::hideleg hideleg;
            /* Hypervisor counter enable. */
            riscv::csr::hcounteren hcounteren;
            /* Hypervisor guest address translation and protection. */
            riscv::csr::hgatp hgatp;
            /* Delta for VS/VU-mode timer. */
            riscv::csr::htimedelta htimedelta;
            /* Upper 32 bits of  htimedelta, RV32I only. */
            riscv::csr::htimedeltah htimedeltah;
            /* Virtual supervisor status register. */
            riscv::csr::vsstatus vsstatus;
            /* Virtual supervisor interrupt-enable register. */
            riscv::csr::vsie vsie;
            /* Virtual supervisor trap handler base address. */
            riscv::csr::vstvec vstvec;
            /* Virtual supervisor scratch register. */
            riscv::csr::vsscratch vsscratch;
            /* Virtual supervisor exception program counter. */
            riscv::csr::vsepc vsepc;
            /* Virtual supervisor trap cause. */
            riscv::csr::vscause vscause;
            /* Virtual supervisor bad address or instruction. */
            riscv::csr::vstval vstval;
            /* Virtual supervisor interrupt pending. */
            riscv::csr::vsip vsip;
            /* Virtual supervisor address translation and protection. */
            riscv::csr::vsatp vsatp;
            /* Base register. */
            riscv::csr::mbase mbase;
            /* Bound register. */
            riscv::csr::mbound mbound;
            /* Instruction base register. */
            riscv::csr::mibase mibase;
            /* Instruction bound register. */
            riscv::csr::mibound mibound;
            /* Data base register. */
            riscv::csr::mdbase mdbase;
            /* Data bound register. */
            riscv::csr::mdbound mdbound;
            /* Physical memory protection configuration. */
            riscv::csr::pmpcfg0 pmpcfg0;
            /* Physical memory protection configuration, RV32 only. */
            riscv::csr::pmpcfg1 pmpcfg1;
            /* Physical memory protection configuration. */
            riscv::csr::pmpcfg2 pmpcfg2;
            /* Physical memory protection configuration, RV32 only. */
            riscv::csr::pmpcfg3 pmpcfg3;
            /* Physical memory protection address register. */
            riscv::csr::pmpaddr0 pmpaddr0;
            /* Physical memory protection address register. */
            riscv::csr::pmpaddr1 pmpaddr1;
            /* Physical memory protection address register. */
            riscv::csr::pmpaddr15 pmpaddr15;
            /* Machine performance-monitoring counter. */
            riscv::csr::mhpmcounter4 mhpmcounter4;
            /* Machine performance-monitoring counter. */
            riscv::csr::mhpmcounter31 mhpmcounter31;
            /* Upper 32 bits of  mcycle, RV32I only. */
            riscv::csr::mcycleh mcycleh;
            /* Upper 32 bits of  minstret, RV32I only. */
            riscv::csr::minstreth minstreth;
            /* Upper 32 bits of  mhpmcounter3, RV32I only. */
            riscv::csr::mhpmcounter3h mhpmcounter3h;
            /* Upper 32 bits of  mhpmcounter4, RV32I only. */
            riscv::csr::mhpmcounter4h mhpmcounter4h;
            /* Upper 32 bits of  mhpmcounter31, RV32I only. */
            riscv::csr::mhpmcounter31h mhpmcounter31h;
            /* Machine performance-monitoring event selector. */
            riscv::csr::mhpmevent4 mhpmevent4;
            /* Machine performance-monitoring event selector. */
            riscv::csr::mhpmevent31 mhpmevent31;
            /* Debug/Trace trigger register select. */
            riscv::csr::tselect tselect;
            /* First Debug/Trace trigger data register. */
            riscv::csr::tdata1 tdata1;
            /* Second Debug/Trace trigger data register. */
            riscv::csr::tdata2 tdata2;
            /* Third Debug/Trace trigger data register. */
            riscv::csr::tdata3 tdata3;
            /* Debug control and status register. */
            riscv::csr::dcsr dcsr;
            /* Debug PC. */
            riscv::csr::dpc dpc;
            /* Debug scratch register 0. */
            riscv::csr::dscratch0 dscratch0;
            /* Debug scratch register 1. */
            riscv::csr::dscratch1 dscratch1;
            /* Hypervisor interrupt-enable register. */
            riscv::csr::hie hie;
            /* Hypervisor guest external interrupt-enable register. */
            riscv::csr::hgeie hgeie;
            /* Hypervisor bad guest physical address. */
            riscv::csr::htval htval;
            /* Hypervisor interrupt pending. */
            riscv::csr::hip hip;
            /* Hypervisor trap instruction (transformed). */
            riscv::csr::htinst htinst;
            /* Hypervisor guest external interrupt pending. */
            riscv::csr::hgeip hgeip;
            /* Machine trap instruction (transformed). */
            riscv::csr::mtinst mtinst;
            /* Machine bad guest physical address. */
            riscv::csr::mtval2 mtval2;
        };

    } /* csr */

    static csr::all csrs;

} /* riscv */

#endif // #define RISCV_CSR_HPP
