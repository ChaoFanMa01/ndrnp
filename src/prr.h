/*
 * In this program, the wireless link quality is measured in 
 * terms of Packet Reception Rate (PRR) based on the log-norm
 * path loss model.
 */

#ifndef NDRNP_PRR_H
#define NDRNP_PRR_H

#include <cmath>

namespace ndrnp {
    // total bits to be sent
    const double BITS = 200;
    // transmit power (in dBm)
//    const double pt = -15;
    // path loss exponent
    const double ple = 4;
    // data rate (in kbps)
    const double dr = 250;
    // noise bandwidth (in kHz)
    const double nb = 50;
    // standard deviation due to multipath effects
    const double std = 5;
    // average path loss at reference distance (in dBm)
    const double pl0 = 20;
    // noise floor (in dBm)
    const double nf = -115;
    // reference distance (in m)
    const double d0 = 1;

    /* @fn snr
     * Compute the average SNR at distance d with 
     * set transmit power to pt.
     */
    double snr(double pt, double d) {
        return pt - pl0 - 10 * ple * std::log10(d / d0) - nf;
    }
    /* @fn q_func
     * Implementation of the Q-function.
     */
    double q_func(double value) {
        return 1.0 - 0.5 * std::erfc(-value * M_SQRT1_2);
    }
    /* @fn ber
     * Compute the bit error rate at distance d with
     * transmit power set to pt.
     */
    double ber(double pt, double d) {
        return q_func(std::sqrt(2 * snr(pt, d) * nb / dr));
    }
    /* @fn prr
     * Compute the packet reception rate at distance d
     * with transmit power set to pt.
     */
    double prr(double pt, double d) {
        double p = - pt;
        return std::pow(1.0 - ber(p, d), 8 * BITS);
    }
}

#endif
