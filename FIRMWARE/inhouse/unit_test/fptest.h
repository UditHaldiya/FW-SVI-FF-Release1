//Table of units ids
typedef struct utable_t
{
    UnitsId_t uid;
    const u8 *uid_name;
    s32 lo;
    s32 hi;
} utable_t;

extern const utable_t utab[];
