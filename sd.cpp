#include "sd.h"

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};
void __cxa_pure_virtual(void) {};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    TIMER2B,
    NOT_ON_TIMER,
    TIMER0B,
    TIMER0A,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    TIMER1A,
    TIMER1B,
    TIMER2A,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
    NOT_ON_TIMER,
};

File::File(SdFile f, const char *n)
{
    _file = (SdFile *)malloc(sizeof(SdFile));

    if (_file)
    {
        memcpy(_file, &f, sizeof(SdFile));
        strncpy(_name, n, 12);
        _name[12] = 0;
    }
}

uint8_t File::isDirectory(void) {
  return (_file && _file->isDir());
}

size_t File::write(const uint8_t *buf, size_t size)
{
  size_t t;
  if (!_file) {
    setWriteError();
    return 0;
  }
  _file->clearWriteError();
  t = _file->write(buf, size);
  if (_file->getWriteError()) {
    setWriteError();
    return 0;
  }
  return t;
}

uint32_t SdVolume::cacheBlockNumber_ = 0XFFFFFFFF;
cache_t SdVolume::cacheBuffer_;
Sd2Card* SdVolume::sdCard_;
uint8_t SdVolume::cacheDirty_ = 0;
uint32_t SdVolume::cacheMirrorBlock_ = 0;

uint8_t SdVolume::allocContiguous(uint32_t count, uint32_t* curCluster)
{
    uint32_t bgnCluster;
    uint8_t setStart;

    if (*curCluster) {
        bgnCluster = *curCluster + 1;
        setStart = false;
    } else {
        bgnCluster = allocSearchStart_;
        setStart = 1 == count;
    }
    uint32_t endCluster = bgnCluster;
    uint32_t fatEnd = clusterCount_ + 1;
    for (uint32_t n = 0;; n++, endCluster++) {
    if (n >= clusterCount_) return false;
    if (endCluster > fatEnd) {
      bgnCluster = endCluster = 2;
    }
    uint32_t f;
    if (!fatGet(endCluster, &f)) return false;

    if (f != 0) {
      bgnCluster = endCluster + 1;
    } else if ((endCluster - bgnCluster + 1) == count) {
      break;
    }
  }
  if (!fatPutEOC(endCluster)) return false;

  while (endCluster > bgnCluster) {
    if (!fatPut(endCluster - 1, endCluster)) return false;
    endCluster--;
  }
  if (*curCluster != 0) {
    if (!fatPut(*curCluster, bgnCluster)) return false;
  }
  *curCluster = bgnCluster;

  if (setStart) allocSearchStart_ = bgnCluster + 1;

  return true;
}

uint8_t SdFile::seekSet(uint32_t pos) {
  if (!isOpen() || pos > fileSize_) return false;

  if (type_ == FAT_FILE_TYPE_ROOT16) {
    curPosition_ = pos;
    return true;
  }
  if (pos == 0) {
    curCluster_ = 0;
    curPosition_ = 0;
    return true;
  }
  uint32_t nCur = (curPosition_ - 1) >> (vol_->clusterSizeShift_ + 9);
  uint32_t nNew = (pos - 1) >> (vol_->clusterSizeShift_ + 9);

  if (nNew < nCur || curPosition_ == 0) {
    curCluster_ = firstCluster_;
  } else {
    nNew -= nCur;
  }
  while (nNew--) {
    if (!vol_->fatGet(curCluster_, &curCluster_)) return false;
  }
  curPosition_ = pos;
  return true;
}

int File::peek() {
  if (! _file)
    return 0;

  int c = _file->read();
  if (c != -1) _file->seekCur(-1);
  return c;
}

size_t Stream::readBytes(char *buffer, size_t length)
{
    size_t count = 0;
    while (count < length)
    {
        int c = timedRead();
        if (c < 0) break;
        *buffer++ = (char)c;
        count++;
    }
    return count;
}

size_t Stream::readBytesUntil(char terminator, char *buffer, size_t length)
{
  if (length < 1) return 0;
  size_t index = 0;
  while (index < length) {
    int c = timedRead();
    if (c < 0 || c == terminator) break;
    *buffer++ = (char)c;
    index++;
  }
  return index;
}


bool Stream::findUntil(char *target, char *terminator)
{
    return findUntil(target, strlen(target), terminator, strlen(terminator));
}

bool Stream::findUntil(char *target, size_t targetLen, char *terminator, size_t termLen)
{
  size_t index = 0;
  size_t termIndex = 0;
  int c;

  if( *target == 0)
        return true;
  while( (c = timedRead()) > 0){

    if(c != target[index])
      index = 0;

    if( c == target[index]){
      if(++index >= targetLen){
        return true;
      }
    }

    if(termLen > 0 && c == terminator[termIndex]){
      if(++termIndex >= termLen)
        return false;
    }
    else
      termIndex = 0;
  }
  return false;
}

int File::read() {
  if (_file)
    return _file->read();
  return -1;
}

int File::read(void *buf, uint16_t nbyte) {
  if (_file)
    return _file->read(buf, nbyte);
  return 0;
}

int File::available() {
  if (! _file) return 0;
  uint32_t n = size() - position();
  return n > 0X7FFF ? 0X7FFF : n;
}

uint8_t File::seek(uint32_t pos) {
  if (! _file) return false;
  return _file->seekSet(pos);
}

uint32_t File::position() {
  if (! _file) return -1;
  return _file->curPosition();
}

uint32_t File::size() {
  if (! _file) return 0;
  return _file->fileSize();
}

void File::close() {
  if (_file) {
    _file->close();
    free(_file);
    _file = 0;
  }
}

File::operator bool() {
  if (_file)
    return  _file->isOpen();
  return false;
}

int Stream::timedRead()
{
  int c;
  _startMillis = millis();
  do {
    c = read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1;
}

int Stream::timedPeek()
{
  int c;
  _startMillis = millis();
  do {
    c = peek();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1;
}

uint8_t SdFile::truncate(uint32_t length) {
  if (!isFile() || !(flags_ & O_WRITE)) return false;

  if (length > fileSize_) return false;

  if (fileSize_ == 0) return true;

  uint32_t newPos = curPosition_ > length ? length : curPosition_;

  if (!seekSet(length)) return false;

  if (length == 0) {
    if (!vol_->freeChain(firstCluster_)) return false;
    firstCluster_ = 0;
  } else {
    uint32_t toFree;
    if (!vol_->fatGet(curCluster_, &toFree)) return false;

    if (!vol_->isEOC(toFree)) {
      if (!vol_->freeChain(toFree)) return false;

      if (!vol_->fatPutEOC(curCluster_)) return false;
    }
  }
  fileSize_ = length;
  flags_ |= F_FILE_DIR_DIRTY;
  if (!sync()) return false;
  return seekSet(newPos);
}

uint8_t SdVolume::freeChain(uint32_t cluster) {
  allocSearchStart_ = 2;

  do {
    uint32_t next;
    if (!fatGet(cluster, &next)) return false;
    if (!fatPut(cluster, 0)) return false;
    cluster = next;
  } while (!isEOC(cluster));

  return true;
}

uint8_t SdVolume::init(Sd2Card* dev, uint8_t part) {
  uint32_t volumeStartBlock = 0;
  sdCard_ = dev;
  if (part) {
    if (part > 4)return false;
    if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
    part_t* p = &cacheBuffer_.mbr.part[part-1];
    if ((p->boot & 0X7F) !=0  ||
      p->totalSectors < 100 ||
      p->firstSector == 0) {
      return false;
    }
    volumeStartBlock = p->firstSector;
  }
  if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
  bpb_t* bpb = &cacheBuffer_.fbs.bpb;
  if (bpb->bytesPerSector != 512 ||
    bpb->fatCount == 0 ||
    bpb->reservedSectorCount == 0 ||
    bpb->sectorsPerCluster == 0) {
      return false;
  }
  fatCount_ = bpb->fatCount;
  blocksPerCluster_ = bpb->sectorsPerCluster;
  clusterSizeShift_ = 0;
  while (blocksPerCluster_ != (1 << clusterSizeShift_)) {
    if (clusterSizeShift_++ > 7) return false;
  }
  blocksPerFat_ = bpb->sectorsPerFat16 ?  bpb->sectorsPerFat16 : bpb->sectorsPerFat32;
  fatStartBlock_ = volumeStartBlock + bpb->reservedSectorCount;
  rootDirEntryCount_ = bpb->rootDirEntryCount;
  rootDirStart_ = fatStartBlock_ + bpb->fatCount * blocksPerFat_;
  dataStartBlock_ = rootDirStart_ + ((32 * bpb->rootDirEntryCount + 511)/512);
  uint32_t totalBlocks = bpb->totalSectors16 ?  bpb->totalSectors16 : bpb->totalSectors32;
  clusterCount_ = totalBlocks - (dataStartBlock_ - volumeStartBlock);
  clusterCount_ >>= clusterSizeShift_;

  if (clusterCount_ < 4085) {
    fatType_ = 12;
  } else if (clusterCount_ < 65525) {
    fatType_ = 16;
  } else {
    rootDirStart_ = bpb->fat32RootCluster;
    fatType_ = 32;
  }
  return true;
}



int Stream::peekNextDigit()
{
  int c;
  while (1) {
    c = timedPeek();
    if (c < 0) return c;
    if (c == '-') return c;
    if (c >= '0' && c <= '9') return c;
    read();
  }
}

size_t Print::print(long n, int base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      return printNumber(n, 10) + t;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}

size_t Print::print(unsigned long n, int base)
{
    if (base == 0) return write(n);
    else return printNumber(n, base);
}

size_t Print::println(void)
{
    size_t n = print('\r');
    n += print('\n');
    return n;
}

size_t Print::println(const char c[])
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(char c)
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
    n += write(*buffer++);
  }
  return n;
}

size_t Print::println(unsigned char b, int base)
{
  size_t n = print(b, base);
  n += println();
  return n;
}

size_t Print::println(int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(unsigned int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

dir_t* SdFile::cacheDirEntry(uint8_t action)
{
  if (!SdVolume::cacheRawBlock(dirBlock_, action)) return NULL;
  return SdVolume::cacheBuffer_.dir + dirIndex_;
}

size_t Print::printFloat(double number, uint8_t digits)
{
  size_t n = 0;

  if (isnan(number)) return print("nan");
  if (isinf(number)) return print("inf");
  if (number > 4294967040.0) return print ("ovf");
  if (number <-4294967040.0) return print ("ovf");

  if (number < 0.0)
  {
     n += print('-');
     number = -number;
  }

  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += print(int_part);

  if (digits > 0) {
    n += print(".");
  }

  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    n += print(toPrint);
    remainder -= toPrint;
  }

  return n;
}

const uint16_t PROGMEM port_to_mode_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &DDRB,
    (uint16_t) &DDRC,
    (uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &PORTB,
    (uint16_t) &PORTC,
    (uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
    NOT_A_PORT,
    NOT_A_PORT,
    (uint16_t) &PINB,
    (uint16_t) &PINC,
    (uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PB,
    PB,
    PB,
    PB,
    PB,
    PB,
    PC,
    PC,
    PC,
    PC,
    PC,
    PC,
};

dir_t* SdFile::readDirCache(void) {
  if (!isDir()) return NULL;
  uint8_t i = (curPosition_ >> 5) & 0XF;
  if (read() < 0) return NULL;
  curPosition_ += 31;
  return (SdVolume::cacheBuffer_.dir + i);
}

uint8_t SdFile::remove(void) {
  if (!truncate(0)) return false;
  dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
  if (!d) return false;
  d->name[0] = DIR_NAME_DELETED;
  type_ = FAT_FILE_TYPE_CLOSED;
  return SdVolume::cacheFlush();
}

uint8_t SdFile::remove(SdFile* dirFile, const char* fileName) {
  SdFile file;
  if (!file.open(dirFile, fileName, O_WRITE)) return false;
  return file.remove();
}

uint8_t SdFile::rmDir(void) {
  if (!isSubDir()) return false;

  rewind();

  while (curPosition_ < fileSize_) {
    dir_t* p = readDirCache();
    if (p == NULL) return false;
    if (p->name[0] == DIR_NAME_FREE) break;
    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;
    if (DIR_IS_FILE_OR_SUBDIR(p)) return false;
  }
  type_ = FAT_FILE_TYPE_NORMAL;
  flags_ |= O_WRITE;
  return remove();
}

size_t Print::println(unsigned long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Print::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}

size_t Print::printNumber(unsigned long n, uint8_t base) {
    char buf[8 * sizeof(long) + 1];
    char *str = &buf[sizeof(buf) - 1];
    *str = '\0';
    if (base < 2) base = 10;

    do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while(n);

    return write(str);
}

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(6),
    _BV(7),
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(0),
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
};

uint8_t SdFile::createContiguous(SdFile* dirFile,
        const char* fileName, uint32_t size)
{
    if (size == 0) return false;
    if (!open(dirFile, fileName, O_CREAT | O_EXCL | O_RDWR)) return false;
    uint32_t count = ((size - 1) >> (vol_->clusterSizeShift_ + 9)) + 1;

    if (!vol_->allocContiguous(count, &firstCluster_))
    {
        remove();
        return false;
    }
    fileSize_ = size;
    flags_ |= F_FILE_DIR_DIRTY;
    return sync();
}

uint8_t SdFile::dirEntry(dir_t* dir)
{
    if (!sync()) return false;
    dir_t* p = cacheDirEntry(SdVolume::CACHE_FOR_READ);
    if (!p) return false;
    memcpy(dir, p, sizeof(dir_t));
    return true;
}

void SdFile::dirName(const dir_t& dir, char* name) {
  uint8_t j = 0;
  for (uint8_t i = 0; i < 11; i++) {
    if (dir.name[i] == ' ')continue;
    if (i == 8) name[j++] = '.';
    name[j++] = dir.name[i];
  }
  name[j] = 0;
}

void pinMode(uint8_t pin, uint8_t mode)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg, *out;

    if (port == NOT_A_PIN) return;

    reg = portModeRegister(port);
    out = portOutputRegister(port);

    if (mode == INPUT) {
        uint8_t oldSREG = SREG;
                cli();
        *reg &= ~bit;
        *out &= ~bit;
        SREG = oldSREG;
    } else if (mode == INPUT_PULLUP) {
        uint8_t oldSREG = SREG;
                cli();
        *reg &= ~bit;
        *out |= bit;
        SREG = oldSREG;
    } else {
        uint8_t oldSREG = SREG;
                cli();
        *reg |= bit;
        SREG = oldSREG;
    }
}

uint8_t SDClass::begin(uint8_t csPin)
{
    return card.init(SPI_HALF_SPEED, csPin) && volume.init(card) && root.openRoot(volume);
}

uint8_t SdVolume::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
  if (cacheBlockNumber_ != blockNumber) {
    if (!cacheFlush()) return false;
    if (!sdCard_->readBlock(blockNumber, cacheBuffer_.data)) return false;
    cacheBlockNumber_ = blockNumber;
  }
  cacheDirty_ |= action;
  return true;
}

uint8_t SdVolume::cacheZeroBlock(uint32_t blockNumber) {
  if (!cacheFlush()) return false;

  for (uint16_t i = 0; i < 512; i++) {
    cacheBuffer_.data[i] = 0;
  }
  cacheBlockNumber_ = blockNumber;
  cacheSetDirty();
  return true;
}
uint8_t SdVolume::chainSize(uint32_t cluster, uint32_t* size) const {
  uint32_t s = 0;
  do {
    if (!fatGet(cluster, &cluster)) return false;
    s += 512UL << clusterSizeShift_;
  } while (!isEOC(cluster));
  *size = s;
  return true;
}
uint8_t SdVolume::fatGet(uint32_t cluster, uint32_t* value) const {
  if (cluster > (clusterCount_ + 1)) return false;
  uint32_t lba = fatStartBlock_;
  lba += fatType_ == 16 ? cluster >> 8 : cluster >> 7;
  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  }
  if (fatType_ == 16) {
    *value = cacheBuffer_.fat16[cluster & 0XFF];
  } else {
    *value = cacheBuffer_.fat32[cluster & 0X7F] & FAT32MASK;
  }
  return true;
}

SdFile SDClass::getParentDir(const char *filepath, int *index)
{
    SdFile d1 = root;
    SdFile d2;
    SdFile *parent = &d1;
    SdFile *subdir = &d2;
    const char *origpath = filepath;

  while (strchr(filepath, '/')) {

    if (filepath[0] == '/') {
      filepath++;
      continue;
    }

    if (! strchr(filepath, '/'))
      break;

    uint8_t idx = strchr(filepath, '/') - filepath;

    if (idx > 12)
      idx = 12;

    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    subdir->close();
    if (! subdir->open(parent, subdirname, O_READ)) {
      return SdFile();
    }
    filepath += idx;
    parent->close();
    SdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  return *parent;
}

long Stream::parseInt(char skipChar)
{
  boolean isNegative = false;
  long value = 0;
  int c;

  c = peekNextDigit();
  if(c < 0)
    return 0;

  do{
    if(c == skipChar)
      ;
    else if(c == '-')
      isNegative = true;
    else if(c >= '0' && c <= '9')
      value = value * 10 + c - '0';
    read();
    c = timedPeek();
  }
  while( (c >= '0' && c <= '9') || c == skipChar );

  if(isNegative)
    value = -value;
  return value;
}

File SDClass::open(const char *filepath, uint8_t mode)
{
    int pathidx;
    SdFile parentdir = getParentDir(filepath, &pathidx);
    filepath += pathidx;

    if (! filepath[0])
        return File(parentdir, "/");

    SdFile file;

    if (!parentdir.isOpen())
        return File();

  if (parentdir.isRoot()) {
    if ( ! file.open(root, filepath, mode)) {
      return File();
    }
  } else {
    if ( ! file.open(parentdir, filepath, mode)) {
      return File();
    }
    parentdir.close();
  }

  if (mode & (O_APPEND | O_WRITE))
    file.seekSet(file.fileSize());
  return File(file, filepath);
}

static void spiSend(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)));
}

static uint8_t spiRec(void)
{
    spiSend(0XFF);
    return SPDR;
}

File File::openNextFile(uint8_t mode) {
  dir_t p;

  while (_file->readDir(&p) > 0) {

    if (p.name[0] == DIR_NAME_FREE) {
      return File();
    }

    if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') {
      continue;
    }

    if (!DIR_IS_FILE_OR_SUBDIR(&p))
        continue;

    SdFile f;
    char name[13];
    _file->dirName(p, name);

    if (f.open(_file, name, mode)) {
      return File(f, name);
    } else {
      return File();
    }
  }

  return File();
}

uint8_t SdVolume::cacheFlush(void) {
  if (cacheDirty_) {
    if (!sdCard_->writeBlock(cacheBlockNumber_, cacheBuffer_.data)) {
      return false;
    }
    if (cacheMirrorBlock_) {
      if (!sdCard_->writeBlock(cacheMirrorBlock_, cacheBuffer_.data)) {
        return false;
      }
      cacheMirrorBlock_ = 0;
    }
    cacheDirty_ = 0;
  }
  return true;
}

uint8_t SdVolume::fatPut(uint32_t cluster, uint32_t value) {
  if (cluster < 2) return false;

  if (cluster > (clusterCount_ + 1)) return false;

  uint32_t lba = fatStartBlock_;
  lba += fatType_ == 16 ? cluster >> 8 : cluster >> 7;

  if (lba != cacheBlockNumber_) {
    if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  }
  if (fatType_ == 16) {
    cacheBuffer_.fat16[cluster & 0XFF] = value;
  } else {
    cacheBuffer_.fat32[cluster & 0X7F] = value;
  }
  cacheSetDirty();

  if (fatCount_ > 1) cacheMirrorBlock_ = lba + blocksPerFat_;
  return true;
}

uint8_t Sd2Card::writeBlock(uint32_t blockNumber, const uint8_t* src) {
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }

  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD24, blockNumber)) {
    error(SD_CARD_ERROR_CMD24);
    goto fail;
  }
  if (!writeData(DATA_START_BLOCK, src)) goto fail;

  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_TIMEOUT);
    goto fail;
  }
  if (cardCommand(CMD13, 0) || spiRec()) {
    error(SD_CARD_ERROR_WRITE_PROGRAMMING);
    goto fail;
  }
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::writeData(const uint8_t* src) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) {
    error(SD_CARD_ERROR_WRITE_MULTIPLE);
    chipSelectHigh();
    return false;
  }
  return writeData(WRITE_MULTIPLE_TOKEN, src);
}

uint8_t Sd2Card::writeData(uint8_t token, const uint8_t* src)
{
    SPDR = token;

    for (uint16_t i = 0; i < 512; i += 2) {
        while (!(SPSR & (1 << SPIF)));
        SPDR = src[i];
        while (!(SPSR & (1 << SPIF)));
        SPDR = src[i+1];
    }

  while (!(SPSR & (1 << SPIF)));

  spiSend(0xff);
  spiSend(0xff);
  status_ = spiRec();
  if ((status_ & DATA_RES_MASK) != DATA_RES_ACCEPTED) {
    error(SD_CARD_ERROR_WRITE);
    chipSelectHigh();
    return false;
  }
  return true;
}

uint8_t Sd2Card::writeStart(uint32_t blockNumber, uint32_t eraseCount) {
  if (blockNumber == 0) {
    error(SD_CARD_ERROR_WRITE_BLOCK_ZERO);
    goto fail;
  }
  if (cardAcmd(ACMD23, eraseCount)) {
    error(SD_CARD_ERROR_ACMD23);
    goto fail;
  }
  if (type() != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
  if (cardCommand(CMD25, blockNumber)) {
    error(SD_CARD_ERROR_CMD25);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}


uint8_t Sd2Card::writeStop(void) {
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  spiSend(STOP_TRAN_TOKEN);
  if (!waitNotBusy(SD_WRITE_TIMEOUT)) goto fail;
  chipSelectHigh();
  return true;
 fail:
  error(SD_CARD_ERROR_STOP_TRAN);
  chipSelectHigh();
  return false;
}


uint8_t Sd2Card::cardCommand(uint8_t cmd, uint32_t arg) {
  readEnd();
  chipSelectLow();
  waitNotBusy(300);
  spiSend(cmd | 0x40);
  for (int8_t s = 24; s >= 0; s -= 8) spiSend(arg >> s);
  uint8_t crc = 0XFF;
  if (cmd == CMD0) crc = 0X95;
  if (cmd == CMD8) crc = 0X87;
  spiSend(crc);
  for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++);
  return status_;
}

uint32_t Sd2Card::cardSize(void)
{
  csd_t csd;
  if (!readCSD(&csd)) return 0;
  if (csd.v1.csd_ver == 0) {
    uint8_t read_bl_len = csd.v1.read_bl_len;
    uint16_t c_size = (csd.v1.c_size_high << 10)
                      | (csd.v1.c_size_mid << 2) | csd.v1.c_size_low;
    uint8_t c_size_mult = (csd.v1.c_size_mult_high << 1)
                          | csd.v1.c_size_mult_low;
    return (uint32_t)(c_size + 1) << (c_size_mult + read_bl_len - 7);
  } else if (csd.v2.csd_ver == 1) {
    uint32_t c_size = ((uint32_t)csd.v2.c_size_high << 16)
                      | (csd.v2.c_size_mid << 8) | csd.v2.c_size_low;
    return (c_size + 1) << 10;
  } else {
    error(SD_CARD_ERROR_BAD_CSD);
    return 0;
  }
}

uint8_t Sd2Card::init(uint8_t sckRateID, uint8_t chipSelectPin)
{
    errorCode_ = inBlock_ = partialBlockRead_ = type_ = 0;
    chipSelectPin_ = chipSelectPin;
    uint16_t t0 = (uint16_t)millis();
    uint32_t arg;
    pinMode(chipSelectPin_, OUTPUT);
    chipSelectHigh();
    pinMode(SPI_MISO_PIN, INPUT);
    pinMode(SPI_MOSI_PIN, OUTPUT);
    pinMode(SPI_SCK_PIN, OUTPUT);
    Pin ss(portB, BIT6);
    ss.direction(OUTPUT);
    ss.set();
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
    SPSR &= ~(1 << SPI2X);
    for (uint8_t i = 0; i < 10; i++) spiSend(0XFF);
    chipSelectLow();

  while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE) {
    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_CMD0);
      goto fail;
    }
  }
  if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND)) {
    type(SD_CARD_TYPE_SD1);
  } else {
    for (uint8_t i = 0; i < 4; i++) status_ = spiRec();
    if (status_ != 0XAA) {
      error(SD_CARD_ERROR_CMD8);
      goto fail;
    }
    type(SD_CARD_TYPE_SD2);
  }
  arg = type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

  while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
    if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT) {
      error(SD_CARD_ERROR_ACMD41);
      goto fail;
    }
  }

  if (type() == SD_CARD_TYPE_SD2) {
    if (cardCommand(CMD58, 0)) {
      error(SD_CARD_ERROR_CMD58);
      goto fail;
    }
    if ((spiRec() & 0XC0) == 0XC0) type(SD_CARD_TYPE_SDHC);
    for (uint8_t i = 0; i < 3; i++) spiRec();
  }
  chipSelectHigh();
  return setSckRate(sckRateID);
fail:
  chipSelectHigh();
  return false;
}

void Sd2Card::readEnd()
{
  if (inBlock_) {
    SPDR = 0XFF;
    while (offset_++ < 513) {
      while (!(SPSR & (1 << SPIF)));
      SPDR = 0XFF;
    }
    while (!(SPSR & (1 << SPIF)));
    chipSelectHigh();
    inBlock_ = 0;
  }
}

uint8_t Sd2Card::readRegister(uint8_t cmd, void* buf) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
  if (cardCommand(cmd, 0)) {
    error(SD_CARD_ERROR_READ_REG);
    goto fail;
  }
  if (!waitStartBlock()) goto fail;
  for (uint16_t i = 0; i < 16; i++) dst[i] = spiRec();
  spiRec();
  spiRec();
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t Sd2Card::setSckRate(uint8_t sckRateID) {
  if (sckRateID > 6) {
    error(SD_CARD_ERROR_SCK_RATE);
    return false;
  }
  if ((sckRateID & 1) || sckRateID == 6) {
    SPSR &= ~(1 << SPI2X);
  } else {
    SPSR |= (1 << SPI2X);
  }
  SPCR &= ~((1 <<SPR1) | (1 << SPR0));
  SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0)
    | (sckRateID & 2 ? (1 << SPR0) : 0);
  return true;
}

uint8_t Sd2Card::waitNotBusy(uint16_t timeoutMillis) {
  uint16_t t0 = millis();
  do {
    if (spiRec() == 0XFF) return true;
  }
  while (((uint16_t)millis() - t0) < timeoutMillis);
  return false;
}
uint8_t Sd2Card::waitStartBlock(void) {
  uint16_t t0 = millis();
  while ((status_ = spiRec()) == 0XFF) {
    if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT) {
      error(SD_CARD_ERROR_READ_TIMEOUT);
      goto fail;
    }
  }
  if (status_ != DATA_START_BLOCK) {
    error(SD_CARD_ERROR_READ);
    goto fail;
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}

int16_t SdFile::read(void* buf, uint16_t nbyte) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(buf);

  if (!isOpen() || !(flags_ & O_READ)) return -1;

  if (nbyte > (fileSize_ - curPosition_)) nbyte = fileSize_ - curPosition_;

  uint16_t toRead = nbyte;
  while (toRead > 0) {
    uint32_t block;
    uint16_t offset = curPosition_ & 0X1FF;
    if (type_ == FAT_FILE_TYPE_ROOT16) {
      block = vol_->rootDirStart() + (curPosition_ >> 9);
    } else {
      uint8_t blockOfCluster = vol_->blockOfCluster(curPosition_);
      if (offset == 0 && blockOfCluster == 0) {
        if (curPosition_ == 0) {
          curCluster_ = firstCluster_;
        } else {
          if (!vol_->fatGet(curCluster_, &curCluster_)) return -1;
        }
      }
      block = vol_->clusterStartBlock(curCluster_) + blockOfCluster;
    }
    uint16_t n = toRead;

    if (n > (512 - offset)) n = 512 - offset;

    if ((unbufferedRead() || n == 512) &&
      block != SdVolume::cacheBlockNumber_) {
      if (!vol_->readData(block, offset, n, dst)) return -1;
      dst += n;
    } else {
      if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_READ)) return -1;
      uint8_t* src = SdVolume::cacheBuffer_.data + offset;
      uint8_t* end = src + n;
      while (src != end) *dst++ = *src++;
    }
    curPosition_ += n;
    toRead -= n;
  }
  return nbyte;
}

int8_t SdFile::readDir(dir_t* dir) {
  int8_t n;
  if (!isDir() || (0X1F & curPosition_)) return -1;

  while ((n = read(dir, sizeof(dir_t))) == sizeof(dir_t)) {
    if (dir->name[0] == DIR_NAME_FREE) break;
    if (dir->name[0] == DIR_NAME_DELETED || dir->name[0] == '.') continue;
    if (DIR_IS_FILE_OR_SUBDIR(dir)) return n;
  }
  return n < 0 ? -1 : 0;
}

uint8_t SdFile::rmRfStar(void) {
  rewind();
  while (curPosition_ < fileSize_) {
    SdFile f;

    uint16_t index = curPosition_/32;

    dir_t* p = readDirCache();
    if (!p) return false;

    if (p->name[0] == DIR_NAME_FREE) break;

    if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') continue;

    if (!DIR_IS_FILE_OR_SUBDIR(p)) continue;

    if (!f.open(this, index, O_READ)) return false;
    if (f.isSubDir()) {
      if (!f.rmRfStar()) return false;
    } else {
      f.flags_ |= O_WRITE;
      if (!f.remove()) return false;
    }
    if (curPosition_ != (32*(index + 1))) {
      if (!seekSet(32*(index + 1))) return false;
    }
  }
  if (isRoot()) return true;
  return rmDir();
}

//SDClass SD;

uint8_t Sd2Card::eraseSingleBlockEnable(void) {
  csd_t csd;
  return readCSD(&csd) ? csd.v1.erase_blk_en : 0;
}

void Sd2Card::partialBlockRead(uint8_t value) {
  readEnd();
  partialBlockRead_ = value;
}

void (*SdFile::dateTime_)(uint16_t* date, uint16_t* time) = NULL;

void (*SdFile::oldDateTime_)(uint16_t& date, uint16_t& time) = NULL;
uint8_t SdFile::addCluster() {
  if (!vol_->allocContiguous(1, &curCluster_)) return false;

  if (firstCluster_ == 0) {
    firstCluster_ = curCluster_;
    flags_ |= F_FILE_DIR_DIRTY;
  }
  return true;
}
uint8_t SdFile::addDirCluster(void) {
  if (!addCluster()) return false;

  uint32_t block = vol_->clusterStartBlock(curCluster_);
  for (uint8_t i = vol_->blocksPerCluster_; i != 0; i--) {
    if (!SdVolume::cacheZeroBlock(block + i - 1)) return false;
  }
  fileSize_ += 512UL << vol_->clusterSizeShift_;
  return true;
}

uint8_t SdFile::close(void) {
  if (!sync())return false;
  type_ = FAT_FILE_TYPE_CLOSED;
  return true;
}

uint8_t SdFile::contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock) {
  if (firstCluster_ == 0) return false;

  for (uint32_t c = firstCluster_; ; c++) {
    uint32_t next;
    if (!vol_->fatGet(c, &next)) return false;

    if (next != (c + 1)) {
      if (!vol_->isEOC(next)) return false;
      *bgnBlock = vol_->clusterStartBlock(firstCluster_);
      *endBlock = vol_->clusterStartBlock(c)
                  + vol_->blocksPerCluster_ - 1;
      return true;
    }
  }
}

uint8_t SdFile::makeDir(SdFile* dir, const char* dirName)
{
    dir_t d;
    if (!open(dir, dirName, O_CREAT | O_EXCL | O_RDWR)) return false;
    flags_ = O_READ;
    type_ = FAT_FILE_TYPE_SUBDIR;
    if (!addDirCluster())return false;
    if (!sync()) return false;
    dir_t* p = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
    if (!p) return false;
    p->attributes = DIR_ATT_DIRECTORY;
    memcpy(&d, p, sizeof(d));
    for (uint8_t i = 1; i < 11; i++) d.name[i] = ' ';
    d.name[0] = '.';
    uint32_t block = vol_->clusterStartBlock(firstCluster_);
    if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_WRITE)) return false;
    memcpy(&SdVolume::cacheBuffer_.dir[0], &d, sizeof(d));
    d.name[1] = '.';

    if (dir->isRoot())
    {
        d.firstClusterLow = 0;
        d.firstClusterHigh = 0;
    }
    else
    {
        d.firstClusterLow = dir->firstCluster_ & 0XFFFF;
        d.firstClusterHigh = dir->firstCluster_ >> 16;
    }

  memcpy(&SdVolume::cacheBuffer_.dir[1], &d, sizeof(d));
  curPosition_ = 2 * sizeof(d);
  return SdVolume::cacheFlush();
}

uint8_t SdFile::open(SdFile* dirFile, const char* fileName, uint8_t oflag)
{
    uint8_t dname[11];
    dir_t* p;
    if (isOpen())return false;
    if (!make83Name(fileName, dname)) return false;
    vol_ = dirFile->vol_;
    dirFile->rewind();
    uint8_t emptyFound = false;

    while (dirFile->curPosition_ < dirFile->fileSize_)
    {
        uint8_t index = 0XF & (dirFile->curPosition_ >> 5);
        p = dirFile->readDirCache();
        if (p == NULL) return false;

    if (p->name[0] == DIR_NAME_FREE || p->name[0] == DIR_NAME_DELETED) {
      if (!emptyFound) {
        emptyFound = true;
        dirIndex_ = index;
        dirBlock_ = SdVolume::cacheBlockNumber_;
      }
      if (p->name[0] == DIR_NAME_FREE) break;
    } else if (!memcmp(dname, p->name, 11)) {
      if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

      return openCachedEntry(0XF & index, oflag);
    }
  }
  if ((oflag & (O_CREAT | O_WRITE)) != (O_CREAT | O_WRITE)) return false;

  if (emptyFound) {
    p = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
    if (!p) return false;
  } else {
    if (dirFile->type_ == FAT_FILE_TYPE_ROOT16) return false;

    if (!dirFile->addDirCluster()) return false;

    dirIndex_ = 0;
    p = SdVolume::cacheBuffer_.dir;
  }
  memset(p, 0, sizeof(dir_t));
  memcpy(p->name, dname, 11);

  if (dateTime_) {
    dateTime_(&p->creationDate, &p->creationTime);
  } else {
    p->creationDate = FAT_DEFAULT_DATE;
    p->creationTime = FAT_DEFAULT_TIME;
  }
  p->lastAccessDate = p->creationDate;
  p->lastWriteDate = p->creationDate;
  p->lastWriteTime = p->creationTime;
  if (!SdVolume::cacheFlush()) return false;
  return openCachedEntry(dirIndex_, oflag);
}

uint8_t Sd2Card::readData(uint32_t block,
        uint16_t offset, uint16_t count, uint8_t* dst) {
  uint16_t n;
  if (count == 0) return true;
  if ((count + offset) > 512) {
    goto fail;
  }
  if (!inBlock_ || block != block_ || offset < offset_) {
    block_ = block;
    if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
    if (cardCommand(CMD17, block)) {
      error(SD_CARD_ERROR_CMD17);
      goto fail;
    }
    if (!waitStartBlock()) {
      goto fail;
    }
    offset_ = 0;
    inBlock_ = 1;
  }

  SPDR = 0XFF;

  for (;offset_ < offset; offset_++) {
    while (!(SPSR & (1 << SPIF)));
    SPDR = 0XFF;
  }
  n = count - 1;
  for (uint16_t i = 0; i < n; i++) {
    while (!(SPSR & (1 << SPIF)));
    dst[i] = SPDR;
    SPDR = 0XFF;
  }
  while (!(SPSR & (1 << SPIF)));
  dst[n] = SPDR;

  offset_ += count;
  if (!partialBlockRead_ || offset_ >= 512) {
    readEnd();
  }
  return true;

 fail:
  chipSelectHigh();
  return false;
}

uint8_t SdFile::make83Name(const char* str, uint8_t* name) {
  uint8_t c;
  uint8_t n = 7;
  uint8_t i = 0;
  while (i < 11) name[i++] = ' ';
  i = 0;
  while ((c = *str++) != '\0') {
    if (c == '.') {
      if (n == 10) return false;
      n = 10;
      i = 8;
    } else {
      PGM_P p = PSTR("|<>^+=?/[];,*\"\\");
      uint8_t b;
      while ((b = pgm_read_byte(p++))) if (b == c) return false;
      if (i > n || c < 0X21 || c > 0X7E)return false;
      name[i++] = c < 'a' || c > 'z' ?  c : c + ('A' - 'a');
    }
  }
  return name[0] != ' ';
}

uint8_t SdFile::open(SdFile* dirFile, uint16_t index, uint8_t oflag) {
  if (isOpen())return false;

  if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) return false;

  vol_ = dirFile->vol_;

  if (!dirFile->seekSet(32 * index)) return false;

  dir_t* p = dirFile->readDirCache();
  if (p == NULL) return false;

  if (p->name[0] == DIR_NAME_FREE ||
      p->name[0] == DIR_NAME_DELETED || p->name[0] == '.') {
    return false;
  }
  return openCachedEntry(index & 0XF, oflag);
}

uint8_t SdFile::openCachedEntry(uint8_t dirIndex, uint8_t oflag) {
  dir_t* p = SdVolume::cacheBuffer_.dir + dirIndex;

  if (p->attributes & (DIR_ATT_READ_ONLY | DIR_ATT_DIRECTORY)) {
    if (oflag & (O_WRITE | O_TRUNC)) return false;
  }
  dirIndex_ = dirIndex;
  dirBlock_ = SdVolume::cacheBlockNumber_;

  firstCluster_ = (uint32_t)p->firstClusterHigh << 16;
  firstCluster_ |= p->firstClusterLow;

  if (DIR_IS_FILE(p)) {
    fileSize_ = p->fileSize;
    type_ = FAT_FILE_TYPE_NORMAL;
  } else if (DIR_IS_SUBDIR(p)) {
    if (!vol_->chainSize(firstCluster_, &fileSize_)) return false;
    type_ = FAT_FILE_TYPE_SUBDIR;
  } else {
    return false;
  }
  flags_ = oflag & (O_ACCMODE | O_SYNC | O_APPEND);

  curCluster_ = 0;
  curPosition_ = 0;

  if (oflag & O_TRUNC) return truncate(0);
  return true;
}

uint8_t SdFile::openRoot(SdVolume* vol) {
  if (isOpen()) return false;

  if (vol->fatType() == 16) {
    type_ = FAT_FILE_TYPE_ROOT16;
    firstCluster_ = 0;
    fileSize_ = 32 * vol->rootDirEntryCount();
  } else if (vol->fatType() == 32) {
    type_ = FAT_FILE_TYPE_ROOT32;
    firstCluster_ = vol->rootDirStart();
    if (!vol->chainSize(firstCluster_, &fileSize_)) return false;
  } else {
    return false;
  }
  vol_ = vol;
  flags_ = O_READ;
  curCluster_ = 0;
  curPosition_ = 0;
  dirBlock_ = 0;
  dirIndex_ = 0;
  return true;
}

uint8_t SdFile::sync(void) {
  if (!isOpen()) return false;

  if (flags_ & F_FILE_DIR_DIRTY) {
    dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
    if (!d) return false;

    if (!isDir()) d->fileSize = fileSize_;

    d->firstClusterLow = firstCluster_ & 0XFFFF;
    d->firstClusterHigh = firstCluster_ >> 16;

    if (dateTime_) {
      dateTime_(&d->lastWriteDate, &d->lastWriteTime);
      d->lastAccessDate = d->lastWriteDate;
    }
    flags_ &= ~F_FILE_DIR_DIRTY;
  }
  return SdVolume::cacheFlush();
}

uint8_t SdFile::timestamp(uint8_t flags, uint16_t year, uint8_t month,
         uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  if (!isOpen()
    || year < 1980
    || year > 2107
    || month < 1
    || month > 12
    || day < 1
    || day > 31
    || hour > 23
    || minute > 59
    || second > 59) {
      return false;
  }
  dir_t* d = cacheDirEntry(SdVolume::CACHE_FOR_WRITE);
  if (!d) return false;

  uint16_t dirDate = FAT_DATE(year, month, day);
  uint16_t dirTime = FAT_TIME(hour, minute, second);
  if (flags & T_ACCESS) {
    d->lastAccessDate = dirDate;
  }
  if (flags & T_CREATE) {
    d->creationDate = dirDate;
    d->creationTime = dirTime;
    d->creationTimeTenths = second & 1 ? 100 : 0;
  }
  if (flags & T_WRITE) {
    d->lastWriteDate = dirDate;
    d->lastWriteTime = dirTime;
  }
  SdVolume::cacheSetDirty();
  return sync();
}

size_t SdFile::write(const void* buf, uint16_t nbyte)
{
    const uint8_t* src = reinterpret_cast<const uint8_t*>(buf);
    uint16_t nToWrite = nbyte;
    if (!isFile() || !(flags_ & O_WRITE)) goto writeErrorReturn;

    if ((flags_ & O_APPEND) && curPosition_ != fileSize_)
    {
        if (!seekEnd())
            goto writeErrorReturn;
    }

  while (nToWrite > 0) {
    uint8_t blockOfCluster = vol_->blockOfCluster(curPosition_);
    uint16_t blockOffset = curPosition_ & 0X1FF;
    if (blockOfCluster == 0 && blockOffset == 0) {
      if (curCluster_ == 0) {
        if (firstCluster_ == 0) {
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = firstCluster_;
        }
      } else {
        uint32_t next;
        if (!vol_->fatGet(curCluster_, &next)) return false;
        if (vol_->isEOC(next)) {
          if (!addCluster()) goto writeErrorReturn;
        } else {
          curCluster_ = next;
        }
      }
    }
    uint16_t n = 512 - blockOffset;

    if (n > nToWrite) n = nToWrite;

    uint32_t block = vol_->clusterStartBlock(curCluster_) + blockOfCluster;
    if (n == 512) {
      if (SdVolume::cacheBlockNumber_ == block) {
        SdVolume::cacheBlockNumber_ = 0XFFFFFFFF;
      }
      if (!vol_->writeBlock(block, src)) goto writeErrorReturn;
      src += 512;
    } else {
      if (blockOffset == 0 && curPosition_ >= fileSize_) {
        if (!SdVolume::cacheFlush()) goto writeErrorReturn;
        SdVolume::cacheBlockNumber_ = block;
        SdVolume::cacheSetDirty();
      } else {
        if (!SdVolume::cacheRawBlock(block, SdVolume::CACHE_FOR_WRITE)) {
          goto writeErrorReturn;
        }
      }
      uint8_t* dst = SdVolume::cacheBuffer_.data + blockOffset;
      uint8_t* end = dst + n;
      while (dst != end) *dst++ = *src++;
    }
    nToWrite -= n;
    curPosition_ += n;
  }
  if (curPosition_ > fileSize_) {
    fileSize_ = curPosition_;
    flags_ |= F_FILE_DIR_DIRTY;
  } else if (dateTime_ && nbyte) {
    flags_ |= F_FILE_DIR_DIRTY;
  }

    if (flags_ & O_SYNC)
    {
        if (!sync()) goto writeErrorReturn;
    }
    return nbyte;

writeErrorReturn:
    setWriteError();
    return 0;
}

uint8_t Sd2Card::erase(uint32_t firstBlock, uint32_t lastBlock)
{
    if (!eraseSingleBlockEnable())
    {
        error(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
        goto fail;
    }

    if (type_ != SD_CARD_TYPE_SDHC) {
    firstBlock <<= 9;
    lastBlock <<= 9;
    }
    if (cardCommand(CMD32, firstBlock)
    || cardCommand(CMD33, lastBlock)
    || cardCommand(CMD38, 0)) {
      error(SD_CARD_ERROR_ERASE);
      goto fail;
  }
  if (!waitNotBusy(SD_ERASE_TIMEOUT)) {
    error(SD_CARD_ERROR_ERASE_TIMEOUT);
    goto fail;
  }
  chipSelectHigh();
  return true;

 fail:
  chipSelectHigh();
  return false;
}

void SdFile::ls(uint8_t flags, uint8_t indent, SerialWrapper &Serial)
{
    dir_t* p;
    rewind();

    while ((p = readDirCache()))
    {
        if (p->name[0] == DIR_NAME_FREE)
            break;

        if (p->name[0] == DIR_NAME_DELETED || p->name[0] == '.')
            continue;

        if (!DIR_IS_FILE_OR_SUBDIR(p))
            continue;

        for (int8_t i = 0; i < indent; i++)
            Serial.print(' ');

        printDirName(*p, flags & (LS_DATE | LS_SIZE) ? 14 : 0, Serial);

        if (flags & LS_DATE)
        {
            printFatDate(p->lastWriteDate, Serial);
            Serial.print(' ');
            printFatTime(p->lastWriteTime, Serial);
        }

        if (!DIR_IS_SUBDIR(p) && (flags & LS_SIZE))
        {
            Serial.print(' ');
            Serial.print(p->fileSize);
        }

        Serial.println();

        if ((flags & LS_R) && DIR_IS_SUBDIR(p))
        {
            uint16_t index = curPosition()/32 - 1;
            SdFile s;

            if (s.open(this, index, O_READ))
                s.ls(flags, indent + 2, Serial);

            seekSet(32 * (index + 1));
        }
    }
}

void SdFile::printDirName(const dir_t& dir, uint8_t width, SerialWrapper &serial)
{
    uint8_t w = 0;

    for (uint8_t i = 0; i < 11; i++)
    {
        if (dir.name[i] == ' ')continue;

        if (i == 8)
        {
            serial.myPutc('.');
            w++;
        }

        serial.myPutc(dir.name[i]);
        w++;
    }

    if (DIR_IS_SUBDIR(&dir))
    {
        serial.myPutc('/');
        w++;
    }

    while (w < width)
    {
        serial.myPutc(' ');
        w++;
    }
}

void SdFile::printFatDate(uint16_t fatDate, SerialWrapper &Serial)
{
    Serial.print(FAT_YEAR(fatDate));
    Serial.print('-');
    printTwoDigits(FAT_MONTH(fatDate), Serial);
    Serial.print('-');
    printTwoDigits(FAT_DAY(fatDate), Serial);
}

void SdFile::printFatTime(uint16_t fatTime, SerialWrapper &Serial)
{
    printTwoDigits(FAT_HOUR(fatTime), Serial);
    Serial.print(':');
    printTwoDigits(FAT_MINUTE(fatTime), Serial);
    Serial.print(':');
    printTwoDigits(FAT_SECOND(fatTime), Serial);
}

void SdFile::printTwoDigits(uint8_t v, SerialWrapper &Serial)
{
  char str[3];
  str[0] = '0' + v/10;
  str[1] = '0' + v % 10;
  str[2] = 0;
  Serial.print(str);
}

void SdFile::oldToNew(uint16_t* date, uint16_t* time)
{
    uint16_t d;
    uint16_t t;
    oldDateTime_(d, t);
    *date = d;
    *time = t;
}

void SdFile::dateTimeCallback(void (*dateTime)(uint16_t* date, uint16_t* time))
{
    dateTime_ = dateTime;
}

uint16_t SdFile::FAT_DATE(uint16_t year, uint8_t month, uint8_t day)
{
    return (year - 1980) << 9 | month << 5 | day;
}

uint8_t SdFile::remove(SdFile& dirFile, const char* fileName)
{
    return remove(&dirFile, fileName);
}

uint8_t SDClass::walkPath(char *filepath, SdFile& parentDir,
        boolean (*callback)(SdFile& parentDir,
        char *filePathComponent, boolean isLastComponent, void *object), void *object)
{
    SdFile subfile1;
    SdFile subfile2;
    char buffer[PATH_COMPONENT_BUFFER_LEN];
    unsigned int offset = 0;
    SdFile *p_parent;
    SdFile *p_child;
    SdFile *p_tmp_sdfile;
    p_child = &subfile1;
    p_parent = &parentDir;

    while (true)
    {
        uint8_t moreComponents = getNextPathComponent(filepath, &offset, buffer);
        uint8_t shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

    if (!shouldContinue) {
      if (p_parent != &parentDir) {
        (*p_parent).close();
      }
      return false;
    }

    if (!moreComponents)
        break;

    uint8_t exists = (*p_child).open(*p_parent, buffer, O_RDONLY);

    if (p_parent != &parentDir) {
      (*p_parent).close();
    }
    if (exists) {
      if (p_parent == &parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
    }
    if (p_parent != &parentDir)
        (*p_parent).close();

    return true;
}

bool SDClass::getNextPathComponent(char *path, unsigned int *p_offset, char *buffer)
{
    int bufferOffset = 0;
    int offset = *p_offset;

    if (path[offset] == '/')
        offset++;

    while (bufferOffset < MAX_COMPONENT_LEN && (path[offset] != '/')  && (path[offset] != '\0'))
        buffer[bufferOffset++] = path[offset++];

    buffer[bufferOffset] = '\0';

    if (path[offset] == '/')
        offset++;

    *p_offset = offset;
    return (path[offset] != '\0');
}




