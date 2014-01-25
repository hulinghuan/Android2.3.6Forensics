#include <stdio.h>

const char * SMS_INFO[3] = {"com.android.providers.telephony/databases/mmssms.db", "mmssms", "sms"};
const char * CONTACTS_INFO[3] = {"com.android.providers.contacts/databases/contacts2.db", "contacts2", "data"};
const char * CALL_LOG_INFO[3] = {"com.sec.android.provider.logsprovider/databases/logs.db", "logs", "logs"};
const char * MEMO_INFO[3] = {"com.sec.android.app.memo/databases/Memo.db", "Memo", "memo"};
const char * BROWSER_HISTORY_INFO[3] = {"com.android.browser/databases/broswer.db", "broswer", "bookmarks"};
const char * BROWSER_USR_PWD_INFO[3] = {"com.android.browser/databases/webview.db", "webview", "password"};
const char * CALENDAR_INFO[3] = {"com.android.providers.calendar/databases/calendar.db", "calander", "event"};
const char * EMAIL_INFO[3] = {"com.android.email/databases/EmailProvider.db", "EmailProvider", "Message"};
const char * EMAIL_USR_PWD_INFO[3] = {"com.android.email/databases/EmailProvider.db", "EmailProvider", "HostAuth"};

const char * WECHAT_VOICE_PATH = {"/sdcard/Tencent/MicroMsg/"};
const char * VEDIO_PATH = {"/sdcard/DCIM/Camera/"};
const char * PICTURE_PATH = {"/sdcard/DCIM/Camera/"};
const char * PULL_PATH = {"/data/data"};

int SaveDbToFile(const char ** db_info);
long FindTableRootPage(unsigned char * raw_content, char * table_name, unsigned long raw_content_length);
unsigned long * FindLostId(unsigned char * raw_content, unsigned long root_page);
unsigned char * FindLostPage(int * id, unsigned char * raw_content);
long Search(unsigned char * target_str, unsigned char * raw_content, unsigned long * begin_offset,unsigned long raw_content_length);
int HuffmanCodeTransition(unsigned char * raw_content, long huffman_offset);
int * ErgodicAllBTreepage(unsigned char * raw_content, int * page_offset, int page_amount);
void ShowByteInform(unsigned char * pBuf, unsigned int uOffset);
