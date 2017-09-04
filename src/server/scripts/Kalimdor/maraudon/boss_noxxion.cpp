/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Noxxion
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */



#define SPELL_TOXICVOLLEY           21687
#define SPELL_UPPERCUT              22916

class boss_noxxion : public CreatureScript
{
public:
    boss_noxxion() : CreatureScript("boss_noxxion")
    { }

    class boss_noxxionAI : public ScriptedAI
    {
        public:
        boss_noxxionAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 ToxicVolley_Timer;
        uint32 Uppercut_Timer;
        uint32 Adds_Timer;
        uint32 Invisible_Timer;
        bool Invisible;
        int Rand;
        int RandX;
        int RandY;
        Creature* Summoned;
    
        void Reset()
        override {
            ToxicVolley_Timer = 7000;
            Uppercut_Timer = 16000;
            Adds_Timer = 19000;
            Invisible_Timer = 15000;                            //Too much too low?
            Invisible = false;
        }
    
        void EnterCombat(Unit *who)
        override {
        }
    
        void SummonAdds(Unit* victim)
        {
            Rand = rand()%8;
            switch (rand()%2)
            {
                case 0: RandX = 0 - Rand; break;
                case 1: RandX = 0 + Rand; break;
            }
            Rand = 0;
            Rand = rand()%8;
            switch (rand()%2)
            {
                case 0: RandY = 0 - Rand; break;
                case 1: RandY = 0 + Rand; break;
            }
            Rand = 0;
            Summoned = DoSpawnCreature(13456, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 90000);
            if(Summoned)
                ((CreatureAI*)Summoned->AI())->AttackStart(victim);
        }
    
        void UpdateAI(const uint32 diff)
        override {
            if (Invisible && Invisible_Timer < diff)
            {
                //Become visible again
                me->SetFaction(FACTION_MONSTER);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                //Noxxion model
                me->SetUInt32Value(UNIT_FIELD_DISPLAYID,11172);
                Invisible = false;
                //me->m_canMove = true;
            } else if (Invisible)
            {
                Invisible_Timer -= diff;
                //Do nothing while invisible
                return;
            }
    
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //ToxicVolley_Timer
            if (ToxicVolley_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_TOXICVOLLEY);
                ToxicVolley_Timer = 9000;
            }else ToxicVolley_Timer -= diff;
    
            //Uppercut_Timer
            if (Uppercut_Timer < diff)
            {
                DoCast(me->GetVictim(),SPELL_UPPERCUT);
                Uppercut_Timer = 12000;
            }else Uppercut_Timer -= diff;
    
            //Adds_Timer
            if (!Invisible && Adds_Timer < diff)
            {
                //Inturrupt any spell casting
                //me->m_canMove = true;
                me->InterruptNonMeleeSpells(false);
                me->SetFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                // Invisible Model
                me->SetUInt32Value(UNIT_FIELD_DISPLAYID,11686);
                SummonAdds(me->GetVictim());
                SummonAdds(me->GetVictim());
                SummonAdds(me->GetVictim());
                SummonAdds(me->GetVictim());
                SummonAdds(me->GetVictim());
                Invisible = true;
                Invisible_Timer = 15000;
    
                Adds_Timer = 40000;
            }else Adds_Timer -= diff;
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_noxxionAI(creature);
    }
};


void AddSC_boss_noxxion()
{
    new boss_noxxion();
}

