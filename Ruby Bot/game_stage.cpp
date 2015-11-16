#include "game_stage.hpp"

#include "opcodes.hpp"
#include "system_info.hpp"

#include "character_data.hpp"

#include "utility.hpp"
#include "main_form.hpp"

namespace maplestory
{
	namespace stage
	{
		game_stage::game_stage(std::size_t index, maple_client* client, maplestory::account& account, maplestory::session& session)
			: stage_base(index), client(client), account(account), session(session)
		{

		}

		game_stage::~game_stage()
		{
			printf("[game_stage] leaving stage\n\n");
		}

		bool game_stage::on_enter_stage()
		{
			printf("[game_stage] entering stage\n");

			unsigned char machine_id[16];
			memset(machine_id, 0, sizeof(machine_id));

			if (!system_info::get_machine_id(machine_id))
			{
				return false;
			}

			packet_writer packet(opcode::out::migrate_in);
			packet.write_dword(this->account.world_id());
			packet.write_dword(this->session.character_id());
			packet.write_buffer(machine_id, sizeof(machine_id));
			packet.write_byte(0x00);
			packet.write_byte(0x00);
			packet.write_qword(this->session.session_id());

			return this->send_packet(packet);
		}
		
		bool game_stage::handle_packet(unsigned short header, packet_reader& packet)
		{
			switch (header)
			{
			case opcode::in::stat_changed:
				return this->on_stat_changed(packet);

			case opcode::in::set_field:
				return this->on_set_field(packet);

			default:
				this->client->get_script_manager().handle_on_packet(header, packet);
				break;
			}

			return true;
		}

		bool game_stage::on_stat_changed(packet_reader& packet)
		{
			packet.read_byte(); // ExclRequestSent flag
			unsigned int stat_flag_1 = packet.read_dword();
			unsigned int stat_flag_2 = packet.read_dword();
			
			if (stat_flag_1 & 1)
			{
				packet.read_byte(); // skin
			}
			if (stat_flag_1 & 2)
			{
				packet.read_dword(); // face
			}
			if (stat_flag_1 & 4)
			{
				packet.read_dword(); // hair
			}
			if (stat_flag_1 & 8)
			{
				packet.indent(8); // aliPetLockerSN[0]
			}
			if (stat_flag_1 & 0x80000)
			{
				packet.indent(8); // aliPetLockerSN[1]
			}
			if (stat_flag_1 & 0x100000)
			{
				packet.indent(8); // aliPetLockerSN[2]
			}
			if (stat_flag_1 & 0x10)
			{
				gui::main_form::get_instance().update_table(this->client->get_index(), "level", static_cast<unsigned int>(packet.read_byte()));
			}
			if (stat_flag_1 & 0x20)
			{
				packet.read_word(); // nJob
			}
			if (stat_flag_1 & 0x40)
			{
				packet.read_word(); // nSTR
			}
			if (stat_flag_1 & 0x80)
			{
				packet.read_word(); // nDEX
			}
			if (stat_flag_1 & 0x100 )
			{
				packet.read_word(); // nINT			
			}
			if (stat_flag_1 & 0x200 )
			{
				packet.read_word(); // nLUK
			}
			if (stat_flag_1 & 0x400 )
			{
				packet.read_dword(); // nHP
			}
			if (stat_flag_1 & 0x800 )
			{
				packet.read_dword(); // nMHP
			}
			if (stat_flag_1 & 0x1000 )
			{
				packet.read_dword(); // nMP
			}
			if (stat_flag_1 & 0x2000 )
			{
				packet.read_dword(); // nMMP
			}
			if (stat_flag_1 & 0x4000 )
			{
				packet.read_word(); // nAP
			}
			if (stat_flag_1 & 0x8000 )
			{
				// nSP -> cbf doing this one.
			}
			if (stat_flag_1 & 0x10000 )
			{
				gui::main_form::get_instance().update_table(this->client->get_index(), "experience", packet.read_qword());
			}
			if (stat_flag_1 & 0x20000 )
			{
				packet.read_word(); // nPOP
			}
			if (stat_flag_1 & 0x40000 )
			{
				gui::main_form::get_instance().update_table(this->client->get_index(), "mesos", packet.read_qword());
			}

			return true;
		}

		bool game_stage::on_set_field(packet_reader& packet)
		{
			auto CClientOptMan__DecodeOpt = [&](packet_reader& packet)
			{
				unsigned int nSize = packet.read_dword();
	
				if (nSize != 0)
				{
					if (nSize > 0)
					{
						for (unsigned int i = 0; i < nSize; i++)
						{
							unsigned int dwType = packet.read_dword();
							unsigned int nValue = packet.read_dword();
						}
					}
		
					packet.read_byte();
				}
			};

			CClientOptMan__DecodeOpt(packet);

			packet.read_dword(); // dwChannelId

			packet.read_byte();
			packet.read_byte();	

			packet.read_dword();
			packet.read_byte(); 
	
			packet.read_dword();
	
			unsigned char bCharacterData = packet.read_byte(); 
			unsigned short nNotifierCheck = packet.read_word();
			
			if (nNotifierCheck != 0)
			{
				std::vector<std::string> asNotifierContent;
				std::string sNotifierTitle = packet.read_string();
		
				if (nNotifierCheck > 0)
				{
					for (unsigned int i = 0; i < nNotifierCheck; i++)
					{
						std::string sNotifierContent = packet.read_string();
						asNotifierContent.push_back(sNotifierContent);
					}
				}
			}
			
			unsigned int map_id = 0;

			if (bCharacterData)
			{
				unsigned int s1 = packet.read_dword();
				unsigned int s2 = packet.read_dword();
				unsigned int s3 = packet.read_dword();
		
				character_data char_data;
				char_data.decode(packet);
				
				gui::main_form::get_instance().update_table(this->client->get_index(), "character_name", this->session.character_name());
				gui::main_form::get_instance().update_table(this->client->get_index(), "level", char_data.level);
				gui::main_form::get_instance().update_table(this->client->get_index(), "experience", char_data.experience);
				gui::main_form::get_instance().update_table(this->client->get_index(), "mesos", char_data.mesos);

				map_id = char_data.map_id;
			}
			else
			{
				unsigned char bRevive = packet.read_byte();
		
				unsigned int dwPosMap = packet.read_dword();
				unsigned char nPortal = packet.read_byte();
				unsigned int nHP = packet.read_dword();
				unsigned char m_bChaseEnable = packet.read_byte();
		
				if (m_bChaseEnable)
				{
					unsigned int m_nTargetPosition_X = packet.read_dword();
					unsigned int m_nTargetPosition_Y = packet.read_dword();
				}

				map_id = dwPosMap;
			}
			
			gui::main_form::get_instance().update_table(this->client->get_index(), "map_id", map_id);
			this->client->get_script_manager().handle_on_map_id(map_id);
			
			return true;
		}
	}
}