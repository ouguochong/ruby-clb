#pragma once

#include "character_info.hpp"

namespace maplestory
{
	typedef struct _character_data
	{
		unsigned int level;
		unsigned __int64 mesos;
		unsigned __int64 experience;
		unsigned int map_id;

		void decode(packet_reader& packet)
		{
			unsigned int dbcharFlag_1 = packet.read_dword();
			unsigned int dbcharFlag_2 = packet.read_dword();
		
			packet.read_byte();
		
			for (unsigned int i = 0; i < 3; i++)
			{
				packet.read_dword();
			}
		
			unsigned char u1 = packet.read_byte();
		
			if (u1 > 0)
			{
				for (unsigned int i = 0; i < u1; i++)
				{
					packet.read_dword();
				}
			}
		
			unsigned int buffer_count = packet.read_dword();
		
			if (buffer_count > 0)
			{
				for (unsigned int i = 0; i < buffer_count; i++)
				{
					packet.read_dword();
					packet.indent(8); // read_buffer
				}
			}
		
			unsigned char u2 = packet.read_byte();
		
			if (u2)
			{
				packet.read_byte(); // Not used in the client itself, only applied for packet alignment. 
			
				for (unsigned int i = 0; i < 2; i++)
				{
					unsigned int u3 = packet.read_dword();
				
					if (u3 > 0)
					{
						for (unsigned int j = 0; j < u3; j++)
						{
							packet.indent(8); // read_buffer
						}
					}
				}
			}
		
			if (dbcharFlag_1 & 0x01)
			{
				character_info char_info;
				char_info.decode(packet);
			
				this->level = char_info.level;
				this->experience = char_info.stat_exp;
				this->map_id = char_info.map_id;

				packet.read_byte();
				packet.read_byte();
			
				for (unsigned int i = 0; i < 3; i++)
				{
					if (packet.read_byte())
					{
						packet.read_string();
					}
				}
			}
		
			if (dbcharFlag_1 & 0x02)
			{
				this->mesos = packet.read_qword();
				packet.read_byte();
				packet.read_dword();
			}
		
			if ((dbcharFlag_1 & 0x08) || (dbcharFlag_1 & 0x8000000))
			{
				unsigned int u5 = packet.read_dword();
			
				if (u5 > 0)
				{
					for (unsigned int i = 0; i < u5; i++)
					{
						packet.read_dword();
						packet.read_dword();
						packet.read_dword();
						packet.read_qword();
					}
				}
			}
		
			for (unsigned int i = 0; i < 5; i++)
			{
				unsigned char inventory_size = 0;
			
				if (dbcharFlag_1 & 0x80)
				{
					inventory_size = packet.read_byte(); // 1C, 1C, 1C, 1C, 60
				}
			
				if (inventory_size)
				{
					/* (Re)Allocate inventory spaces */
				}
			}
		
			if (dbcharFlag_1 & 0x100000)
			{
				packet.read_dword(); // CharacterData->aEquipExtExpire[0].dwLowDateTime
				packet.read_dword(); // CharacterData->aEquipExtExpire[0].dwHighDateTime
			}
		
			/*
			if (dbcharFlag_1 & 0x04)
			{
				// Equipped1 inventory, Equipped2 inventory, Equip inventory tab
				packet.read_byte();
			
				for (unsigned int i = 0; i < 3; i++)
				{
					for (unsigned int j = 0; j < 57; j++)
					{
						// Set inventory slots to zero
					}			
				
					unsigned short initial_slot_id = packet.read_word();
				
					if (initial_slot_id != 0)
					{
						for (unsigned int j = initial_slot_id; (j != 0); j = packet.read_word())
						{						
							GW_ItemSlotBase__Decode(iPacket);
						}
					}
				}
			
				// Android inventory (heart)
				packet.read_byte();
			
				if (packet.read_word())
				{
					GW_ItemSlotBase__Decode(iPacket);
				}
			
				// Dragon inventory
				for (unsigned int i = 0; i < 9; i++)
				{
					unsigned short initial_slot_id = packet.read_word();
				
					if (initial_slot_id != 0)
					{
						for (unsigned int j = initial_slot_id; (j != 0); j = packet.read_word())
						{						
							GW_ItemSlotBase__Decode(iPacket);
						}
					}
				}
			
				// Mechanic inventory
				for (unsigned int i = 0; i < 2; i++)
				{
					unsigned short initial_slot_id = packet.read_word();
				
					if (initial_slot_id != 0)
					{
						for (unsigned int j = initial_slot_id; (j != 0); j = packet.read_word())
						{						
							GW_ItemSlotBase__Decode(iPacket);
						}
					}
				}
			}
		
			// Normal inventories (Use, Etc, Setup, Cash)
			for (unsigned int i = 0, flag = 1; i < 4; i++, flag *= 2)
			{
				if (dbcharFlag_1 & (i * 8))
				{
					unsigned short initial_slot_id = packet.read_byte();
				
					if (initial_slot_id != 0)
					{
						for (unsigned int j = initial_slot_id; (j != 0); j = packet.read_byte())
						{						
							GW_ItemSlotBase__Decode(iPacket);
						}
					}
				}
			}*/
		}
	} character_data;
}