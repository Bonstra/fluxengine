#ifndef SECTOR_H
#define SECTOR_H

/* 
 * Note that sectors here used zero-based numbering throughout (to make the
 * maths easier); traditionally floppy disk use 0-based track numbering and
 * 1-based sector numbering, which makes no sense.
 */
class Sector
{
public:
	enum Status
	{
		OK,
		BAD_CHECKSUM,
        MISSING,
        CONFLICT
	};

    static const std::string statusToString(Status status);

    Sector(int status, int track, int side, int sector, const std::vector<uint8_t>& data):
		status(status),
        track(track),
        side(side),
        sector(sector),
        data(data)
    {}

	int status;
    const int track;
    const int side;
    const int sector;
    const std::vector<uint8_t> data;
};

typedef std::vector<std::unique_ptr<Sector>> SectorVector;

#endif

