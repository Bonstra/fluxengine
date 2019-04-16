#ifndef FLUXREADER_H
#define FLUXREADER_H

class Fluxmap;
class DataSpec;

class FluxReader
{
public:
    virtual ~FluxReader() {}

private:
    static std::unique_ptr<FluxReader> createSqliteFluxReader(const std::string& filename);
    static std::unique_ptr<FluxReader> createHardwareFluxReader(unsigned drive);
    static std::unique_ptr<FluxReader> createStreamFluxReader(const std::string& path);

public:
    static std::unique_ptr<FluxReader> create(const DataSpec& spec);

public:
    virtual std::unique_ptr<Fluxmap> readFlux(int track, int side) = 0;
    virtual void recalibrate() {}
    virtual bool retryable() { return false; }
};

extern void setHardwareFluxReaderRevolutions(int revolutions);
extern void setHardwareFluxReaderDensity(bool high_density);

#endif

