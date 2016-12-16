/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef EIO_BOOT__H
#define EIO_BOOT__H

#define BOOT_SLOT_0_SUFFIX      "i"
#define BOOT_SLOT_1_SUFFIX      "e"

#define BOOT_SLOT_SYSTEM_PREFIX "system"
#define BOOT_SLOT_BOOT_PREFIX   "boot"
#define BOOT_SLOT_COMMANDER     "eio"

#define EIO_BOOT_MAGIC        0x331100BB
#define EIO_BOOT_NUM_ALT_PART 2

#define EIO_BOOT_TRY_ATTEMPT  2

struct eio_boot_slot {
   char suffix[8];
   int  valid;
   int  boot_try;
   int  boot_ok;
   int  boot_fail;
};

struct eio_boot {
   int    magic;
   int    current;
   struct eio_boot_slot slot[EIO_BOOT_NUM_ALT_PART];
};

#endif /* EIO_BOOT__H */
