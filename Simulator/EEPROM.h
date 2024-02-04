#ifndef EEPROM_H
#define EEPROM_H

class QFile;
class EpromStore
{
public:
    EpromStore();
    ~EpromStore();
    char read(int aIndex);
    void write(int aIndex, const char aByte);
private:
    QFile *mFile;
};

extern EpromStore EEPROM;

#endif // EEPROM_H

