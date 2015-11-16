#include "stage_base.hpp"

#include "crc32.hpp"
#include "opcodes.hpp"

#include "main_form.hpp"
#include "maple_client.hpp"

namespace maplestory
{
	namespace stage
	{
		stage_base::stage_base(std::size_t index) 
			: index(index), handshake(false)
		{
			tcp_socket = std::make_unique<network::tcp_socket>();
		}

		stage_base::~stage_base()
		{

		}

		bool stage_base::try_read()
		{
			if (!this->handshake)
			{
				if (!(this->handshake = this->forward_handshake()))
				{
					return false;
				}

				if (!this->on_enter_stage())
				{
					return false;
				}
			}
			else
			{
				std::vector<packet_reader> packets;

				if (!this->recv_packets(packets))
				{
					return false;
				}

				for (packet_reader& packet : packets)
				{
					if (!this->forward_packet(packet))
					{
						return false;
					}
				}
			}
				
			return true;
		}
		
		bool stage_base::connect(std::string const& ip, unsigned short port)
		{
			return this->tcp_socket.get()->try_connect(ip, port);
		}
		
		void stage_base::reset_index(std::size_t index)
		{
			this->index = index;
		}

		SOCKET stage_base::get_desc() const
		{
			return this->tcp_socket.get()->get_desc();
		}
		
		std::size_t stage_base::get_index() const
		{
			return this->index;
		}

		bool stage_base::send_packet(packet_writer& packet)
		{
			unsigned short crc_key = 0;

			while (crc_key = *reinterpret_cast<unsigned short*>(&this->send_iv), (crc_key % 31) == 0)
			{
				packet_writer check_crc_packet(opcode::out::security_packet);
				check_crc_packet.write_byte(0x01);
				check_crc_packet.write_dword(crc32::crc32(reinterpret_cast<unsigned char*>(&crc_key), 2));
				check_crc_packet.write_dword(0x00000000);

				if (!this->tcp_socket.get()->send_packet(check_crc_packet, this->version_major, this->send_iv))
				{
					return false;
				}
			}

			//gui::main_form::get_instance().on_send_packet(this->index, packet);
			return this->tcp_socket.get()->send_packet(packet, this->version_major, this->send_iv);
		}

		bool stage_base::recv_packets(std::vector<packet_reader>& packets)
		{
			return this->tcp_socket.get()->recv_packets(packets, this->recv_iv);
		}
			
		bool stage_base::handle_handshake(unsigned char locale, unsigned short version_major, unsigned short version_minor)
		{
			return true;
		}
		
		bool stage_base::forward_handshake()
		{
			packet_reader packet;
			
			if (!this->tcp_socket.get()->recv_handshake(packet))
			{
				printf("[stage_base::forward_handshake] failed to recv handshake\n");
				return false;
			}
			
			this->version_major = packet.read_word();
			this->version_minor = packet.read_string_as<unsigned short>();

			this->send_iv = packet.read_dword();
			this->recv_iv = packet.read_dword();

			this->locale = packet.read_byte();

			return this->handle_handshake(this->locale, this->version_major, this->version_minor);
		}

		bool stage_base::forward_packet(packet_reader& packet)
		{
			//gui::main_form::get_instance().on_recv_packet(this->index, packet);

			unsigned short header = packet.read_word();

			switch (header)
			{
			case opcode::in::alive_req:
				return this->on_alive_req(packet);

			case opcode::in::security_packet:
				return this->on_security_packet(packet);
					
			default:
				break;
			}
			
			return this->handle_packet(header, packet);
		}
		
		bool stage_base::on_alive_req(packet_reader& packet)
		{
			packet_writer alive_ack(opcode::out::alive_ack);
			alive_ack.write_word(0x0000);
			alive_ack.write_dword(0x00000000);
			alive_ack.write_dword(GetTickCount());

			return this->send_packet(alive_ack);
		}

		bool stage_base::on_security_packet(packet_reader& packet)
		{
			return true;
		}
	}
}