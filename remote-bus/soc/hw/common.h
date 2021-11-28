#ifndef __HW_COMMON_H
#define __HW_COMMON_H

#include <stdint.h>
#include <system.h>

/* To overwrite CSR subregister accessors, define extern, non-inlined versions
 * of csr_[read|write]_simple(), and define CSR_ACCESSORS_DEFINED.
 */

#ifndef CSR_ACCESSORS_DEFINED
#define CSR_ACCESSORS_DEFINED

uint32_t csr_read_simple(unsigned long addr);
void csr_write_simple(uint32_t val, unsigned long addr);

#endif /* ! CSR_ACCESSORS_DEFINED */

#endif /* __HW_COMMON_H */
