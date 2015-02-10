#ifndef GYRO_HPP
#define GYRO_HPP

#include <pthread.h>
#include "WPILib.h"

namespace frc973 {

/*
 * Task that continually checks the gyro and serves that data out
 */

class SPIGyro {
    public:
        /*
         * Constructor initializes gyroscope, starts a thread to continually
         * update values, and then returns.
         */
        SPIGyro(int size);

        /*
         * Returns the latest angle reading from the gyro.
         */
        double GetDegrees();

        /*
         * Returns the the latest angular momentum reading from the gyro.
         */
        double GetDegreesPerSec();

        /*
         * Notify gyro to start shutdown sequence soon.
         */
        void Quit();
    private:
        /*
         * Initializes and zeros the gyro, then starts collecting angular data
         * to be served out by this object.  Should be run in its own thread
         * (started by constructor).  Doesn't stop until someone calls Quit.
         */
        static void* Run(void *p);

        /* 
         * Runs the recommended gyro startup procedure including checking all
         * of the self-test bits.
         * Returns true on success.
         */
        bool InitializeGyro();

        /*
         * Zero the gyroscope by averaging all readings over 2 seconds.
         * If a lot of those ended up in error, at least make sure we have ~.5
         * seconds worth of data before continuing.
         */
        void ZeroAngle();

        /*
         * Pulls the angular rate from the gyro, checks for errors, and then 
         * updates this object to serve out that data.
         */
        void UpdateReading();

        /* 
         * Gets a reading from the gyro.
         * Returns a value to be passed to the Extract* methods or 0 for error.
         */
        uint32_t GetReading();

        /*
         * Reads from the gyro's internal memory and returns the value.
         * Retries until it succeeds.
         */
        uint16_t DoRead(uint8_t address);

        /*
         * Returns all of the non-data bits in the "header" except the parity from
         * value.
         */
        uint8_t ExtractStatus(uint32_t value) { return (value >> 26) & ~4; }

        /*
         * Checks for erros in the passed int.
         * Returns true if there was an error in the reading, false otherwise.
         * Prints messages to explain possible errors.
         * |res| should be the result of calling gyro.GetReading()
         */
        bool CheckErrors(uint32_t res);
        
        /*
         * Returns all of the error bits in the "footer" from value.
         */
        uint8_t ExtractErrors(uint32_t value) { return (value >> 1) & 0x7F; }

        /* 
         * Returns the anglular rate contained in value.
         */
        double ExtractAngle(uint32_t value);

        /* 
         * Performs a transaction with the gyro.
         * to_write is the value to write. This function handles setting the checksum
         * bit.
         * result is where to stick the result. This function verifies the parity bit.
         * Returns true for success.
         */
        bool DoTransaction(uint32_t to_write, uint32_t *result);

        /* 
         * Returns the part ID from the gyro.
         * Retries until it succeeds.
         */
        uint32_t ReadPartID();


        pthread_mutex_t mutex;
        SPI *gyro;
        Timer timer;
        double angle;
        double angularMomentum;
        long timeLastUpdate;
        bool run_;
        double zero_offset;
        
        // Readings per second.
        static const int kReadingRate = 200;
};

}

#endif
