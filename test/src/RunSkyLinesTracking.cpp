/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Tracking/SkyLines/Client.hpp"
#include "NMEA/Info.hpp"
#include "OS/Args.hpp"
#include "Util/NumberParser.hpp"
#include "Util/StringUtil.hpp"
#include "DebugReplay.hpp"
#include "Net/IPv4Address.hxx"
#include "Net/StaticSocketAddress.hxx"
#include "IO/Async/GlobalAsioThread.hpp"

#ifdef HAVE_SKYLINES_TRACKING_HANDLER
#include "Thread/Mutex.hpp"
#include "Thread/Cond.hxx"

#include <boost/asio/steady_timer.hpp>

class Handler : public SkyLinesTracking::Handler {
  SkyLinesTracking::Client &client;

  boost::asio::steady_timer timer;

public:
  explicit Handler(SkyLinesTracking::Client &_client)
    :client(_client), timer(client.get_io_service()) {}

  virtual void OnAck(unsigned id) override {
    printf("received ack %u\n", id);
    client.Close();
  }

  virtual void OnTraffic(unsigned pilot_id, unsigned time_of_day_ms,
                         const GeoPoint &location, int altitude) override {
    BrokenTime time = BrokenTime::FromSecondOfDay(time_of_day_ms / 1000);

    printf("received traffic pilot=%u time=%02u:%02u:%02u location=%f/%f altitude=%d\n",
           pilot_id, time.hour, time.minute, time.second,
           (double)location.longitude.Degrees(),
           (double)location.latitude.Degrees(),
           altitude);

    timer.expires_from_now(std::chrono::seconds(1));
    timer.async_wait([this](const boost::system::error_code &ec){
        if (!ec)
          client.Close();
      });
  }
};

#endif

int
main(int argc, char *argv[])
try {
  Args args(argc, argv, "HOST KEY");
  const char *host = args.ExpectNext();
  const char *key = args.ExpectNext();

  boost::asio::io_service io_service;

  boost::asio::ip::udp::resolver resolver(io_service);
  const auto endpoint = *resolver.resolve({host, "5597"});

  ScopeGlobalAsioThread global_asio_thread;

  SkyLinesTracking::Client client(io_service);

#ifdef HAVE_SKYLINES_TRACKING_HANDLER
  Handler handler(client);
  client.SetHandler(&handler);
#endif

  client.SetKey(ParseUint64(key, NULL, 16));
  if (!client.Open(endpoint)) {
    fprintf(stderr, "Failed to create client\n");
    return EXIT_FAILURE;
  }

  if (args.IsEmpty() || StringIsEqual(args.PeekNext(), "fix")) {
    NMEAInfo basic;
    basic.Reset();
    basic.UpdateClock();
    basic.time = 1;
    basic.time_available.Update(basic.clock);

    return client.SendFix(basic) ? EXIT_SUCCESS : EXIT_FAILURE;
  } else if (StringIsEqual(args.PeekNext(), "ping")) {
    client.SendPing(1);

#ifdef HAVE_SKYLINES_TRACKING_HANDLER
    io_service.run();
  } else if (StringIsEqual(args.PeekNext(), "traffic")) {
    client.SendTrafficRequest(true, true, true);

    io_service.run();
#endif
  } else {
    DebugReplay *replay = CreateDebugReplay(args);
    if (replay == NULL)
      return EXIT_FAILURE;

    while (replay->Next()) {
      client.SendFix(replay->Basic());
      usleep(100000);
    }
  }

  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  fprintf(stderr, "%s\n", e.what());
  return EXIT_FAILURE;
}
