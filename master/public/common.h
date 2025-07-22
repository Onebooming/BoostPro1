#pragma once

#define ERROR_OFFSET_VAL  0x10000

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS                   0
#endif

#ifndef ERROR_FAILED
#define ERROR_FAILED                    (0x01 + ERROR_OFFSET_VAL)
#endif

#ifndef ERROR_INVALID_PARAM
#define ERROR_INVALID_PARAM             (0x02 + ERROR_OFFSET_VAL)
#endif

#ifndef ERROR_NOT_FOUND
#define ERROR_NOT_FOUND                 (0x03 + ERROR_OFFSET_VAL)
#endif

#ifndef ERROR_ALREADY_EXIST
#define ERROR_ALREADY_EXIST              (0x04 + ERROR_OFFSET_VAL)
#endif