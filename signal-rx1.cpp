
#include <iostream>
#include <vector>
#include <complex>

#include <uhd/usrp/multi_usrp.hpp>

std::string to_ascii(const std::vector<bool>& bits)
{
    std::string ret = "";
    for (size_t i = 0; i + 8 <= bits.size(); i += 8) {
        int c = 0;
        for (int k = 0; k < 8; k++) {
            const int x = bits[i+k];
            c = (c << 1) | x;
        }
        ret += static_cast<char>(c);
    }
    return ret;
}

int main()
{
    const double freq = 433.92e6;
    const double sample_rate = 1000000;
    const double gain = 60;
    const std::string antenna = "TX/RX";
    const std::string args = "";

    std::cout << "hello rx\n";
    // get usrp ref
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);

    // init radio settings
    usrp->set_rx_rate(sample_rate);
    usrp->set_rx_freq(freq);
    usrp->set_rx_gain(gain);
    usrp->set_rx_antenna(antenna);

    // check radio settings
    std::cout << "USRP RX RATE: " << usrp->get_rx_rate() << "\n";
    std::cout << "USRP RX FREQ: " << usrp->get_rx_freq() << "\n";
    std::cout << "USRP RX GAIN: " << usrp->get_rx_gain() << "\n";
    std::cout << "USRP RX ANTN: " << usrp->get_rx_antenna() << "\n";

    // get streamer
    uhd::stream_args_t stream_args("fc32","sc16");
    uhd::rx_streamer::sptr rx_stream = usrp->get_rx_stream(stream_args);
    
    // start streaming
    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);
    stream_cmd.num_samps = 5e6;
    stream_cmd.stream_now = true;
    rx_stream->issue_stream_cmd(stream_cmd);

    std::vector<std::complex<float>> buff(5e6);
    uhd::rx_metadata_t rx_metadata;

    std::cout << "start recv...\n";
    int total_rx = 0;
    while (true) {
        int s = rx_stream->recv(buff.data() + total_rx, 1e5, rx_metadata, 10);
        total_rx += s;
        if (total_rx >= 5e6) {
            break;
        }
    }
    std::cout << "vector size: " << buff.size() << "\n";
    std::cout << "first:  " << std::abs(buff[0]) << "\n";
    std::cout << "middle: " << std::abs(buff[500000]) << "\n";
    float min = std::abs(buff[0]);
    float max = std::abs(buff[0]);
    for (auto x : buff) {
        const float r = std::abs(x);
        if (r < min) {
            min = r;
        }
        if (r > max) {
            max = r;
        }
    }
    std::cout << "min: " << min << "\n";
    std::cout << "max: " << max << "\n";

    // process signal
    const float threshold = (min + max) * 0.25;
    int const start_len = (int) (5000 * 0.99);
    int const end_len = (int) (20000 * 0.99);
    int const symbol_len = 1000;

    int on_count = 0;
    int off_count = 0;
    bool in_preable = false;
    bool in_message = false;

    int msg_samples = 0;
    std::vector<bool> msg;

    for (int i = 0; i < (int) buff.size(); i++) {
        const float r = std::abs(buff[i]); // mag

        // check on/off
        if (r < threshold) {
            // off
            on_count = 0;
            off_count++;
        } else {
            // on
            on_count++;
            off_count = 0;
        }

        // STATE CHANGES
        // 1: detect we are in the preable
        if (!in_preable && !in_message && on_count > start_len) {
            in_preable = true;
        }
        // 2: detect preable is done -> we are in_message
        if (in_preable && off_count) {
            in_preable = false;
            in_message = true;
            msg_samples = 0;
        }
        // 3: detect message is over
        if (in_message && off_count > end_len) {
            in_message = false;
        }

        if (in_message) {
            if ((msg_samples + symbol_len/2) % (symbol_len) == 0) {
                msg.push_back(on_count != 0);
            }
            msg_samples++;
        }
    }

    std::string m = to_ascii(msg);
    std::cout << "msg: " << m << "\n";



    
    return 0;
}
