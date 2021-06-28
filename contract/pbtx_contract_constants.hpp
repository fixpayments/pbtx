/*
  Copyright 2021 Fix Payments Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/


/*
  Per-network flags used in "regnetwork" action. Upper 16 bits are
  free to use by listener contracts for their internal needs. Lower 16
  bits are reserved for PBTX own use.
*/
const uint32_t PBTX_FLAGS_PBTX_RESERVED = 0x0000FFFF;
const uint32_t PBTX_FLAGS_PBTX_KNOWN    = 0x00000003;
const uint32_t PBTX_FLAG_RAW_NOTIFY     = 1<<0;        // if set, require_recipient is used in notifications
const uint32_t PBTX_FLAG_HISTORY        = 1<<1;        // if set, history log is written on all events


/*
  Values for event_type field in "history" table.
*/
const uint8_t PBTX_HISTORY_EVENT_NETMETADATA = 1;
const uint8_t PBTX_HISTORY_EVENT_REGACTOR    = 2;
const uint8_t PBTX_HISTORY_EVENT_UNREGACTOR  = 3;
const uint8_t PBTX_HISTORY_EVENT_EXECTRX     = 4;