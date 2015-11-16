#pragma once

#include "packet_reader.hpp"
#include "packet_writer.hpp"

namespace maplestory
{
	namespace network
	{
		class tcp_socket
		{
		public:
			tcp_socket();
			~tcp_socket();

			bool try_connect(std::string const& ip, unsigned short port);
			bool try_disconnect();

			bool recv_handshake(packet_reader& packet);
			bool recv_packets(std::vector<packet_reader>& packets, unsigned int& iv);

			bool send_packet(packet_writer& packet, unsigned short version_major, unsigned int& iv);

			SOCKET get_desc() const;

		private:
			bool raw_recv(unsigned char* buffer, std::size_t size);
			bool raw_send(unsigned char const* buffer, std::size_t size);

			SOCKET sock;

			unsigned int recv_offset;
			unsigned char recv_buffer[1024 * 16];
		};
	}
}