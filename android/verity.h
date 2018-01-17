/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VERITY__H
#define VERITY__H

#include "android_types.h"

#define SECTORSIZE 512
#define SHA256_DIGEST_SIZE (256/8)
#define FEC_BLOCKSIZE 4096
#define FEC_MAGIC 0xFECFECFE
#define RSANUMBYTES 256
#define VERITY_METADATA_MAGIC_NUMBER 0xb001b001
#define VERITY_METADATA_MAGIC_DISABLE 0x46464f56
#define VERITY_METADATA_SIZE (8*FEC_BLOCKSIZE)

typedef struct fec_header {
	__le32 magic;
	__le32 version;
	__le32 size;
	__le32 roots;
	__le32 fec_size;
	__le64 inp_size;
	u8 hash[SHA256_DIGEST_SIZE];
}
#ifdef __GNUC__
__attribute__ ((packed))
#endif
fec_header_t;

typedef struct android_metadata_header {
	__le32 magic_number;
	__le32 protocol_version;
	char signature[RSANUMBYTES];
	__le32 table_length;
} android_metadata_header_t;

#endif /* VERITY__H */
