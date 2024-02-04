#include <EEPROM.h>

#include <QFile>



EpromStore::EpromStore()
{
    mFile = new QFile("./EEPROM.dat");
    mFile->open(QIODevice::ReadWrite);
}

EpromStore::~EpromStore()
{
    mFile->close();
    delete mFile;
}

char EpromStore::read(int aIndex )
{
    char fByte = 0;
    if (mFile->seek(aIndex))
    {
        mFile->read(&fByte, 1);
    }
    return fByte;
}

void EpromStore::write(int aIndex, const char aByte )
{
    if (aIndex == 0)
    {
        mFile->seek(0);
    }
    mFile->write(&aByte, 1);
}


EpromStore EEPROM = EpromStore();
