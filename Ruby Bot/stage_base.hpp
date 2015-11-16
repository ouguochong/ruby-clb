#pragma once

#include <Windows.h>

#include "packet_reader.hpp"
#include "packet_writer.hpp"
#include "tcp_socket.hpp"

#include <memory>

namespace maplestory
{
	namespace stage
	{
		class stage_base
		{
		public:
			stage_base(std::size_t index);
			virtual ~stage_base();

			bool try_read();
			bool connect(std::string const& ip, unsigned short port);

			void reset_index(std::size_t index = 0);

			SOCKET get_desc() const;
			std::size_t get_index() const;

			bool send_packet(packet_writer& packet);

		protected:
			bool recv_packets(std::vector<packet_reader>& packets);
			
			virtual bool on_enter_stage() = 0;

			virtual bool handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor);
			virtual bool handle_packet(unsigned short header, packet_reader& packet) = 0;
			
		private:
			bool forward_handshake();
			bool forward_packet(packet_reader& packet);
			
			bool on_alive_req(packet_reader& packet);
			bool on_security_packet(packet_reader& packet);

		private:
			std::unique_ptr<network::tcp_socket> tcp_socket;
			
			std::size_t index;

			bool handshake;

			unsigned char locale;
			unsigned short version_major;
			unsigned short version_minor;
			unsigned int send_iv;
			unsigned int recv_iv;
		};
	}
}