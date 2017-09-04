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
SDName: Boss_Chrono_Lord_Deja
SD%Complete: 65
SDComment: All abilities not implemented
SDCategory: Caverns of Time, The Dark Portal
EndScriptData */


#include "def_dark_portal.h"

enum eEnums
{
    SAY_ENTER                   = -1269006,
    SAY_AGGRO                   = -1269007,
    SAY_BANISH                  = -1269008,
    SAY_SLAY1                   = -1269009,
    SAY_SLAY2                   = -1269010,
    SAY_DEATH                   = -1269011,

    SPELL_ARCANE_BLAST          = 31457,
    H_SPELL_ARCANE_BLAST        = 38538,
    SPELL_ARCANE_DISCHARGE      = 31472,
    H_SPELL_ARCANE_DISCHARGE    = 38539,
    SPELL_TIME_LAPSE            = 31467,
    SPELL_ATTRACTION            = 38540                       //Not Implemented (Heroic mode)
};


class boss_chrono_lord_deja : public CreatureScript
{
public:
    boss_chrono_lord_deja() : CreatureScript("boss_chrono_lord_deja")
    { }

    class boss_chrono_lord_dejaAI : public ScriptedAI
    {
        public:
        boss_chrono_lord_dejaAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
            HeroicMode = me->GetMap()->IsHeroic();
        }
    
        InstanceScript *pInstance;
        bool HeroicMode;
    
        uint32 ArcaneBlast_Timer;
        uint32 TimeLapse_Timer;
        uint32 Attraction_Timer;
        uint32 ArcaneDischarge_Timer;
    
        void Reset() override
        {
            ArcaneBlast_Timer = 18000+rand()%5000;
            TimeLapse_Timer = 10000+rand()%5000;
            ArcaneDischarge_Timer = 20000+rand()%10000;
            Attraction_Timer = 25000+rand()%10000;
        }
    
        void EnterCombat(Unit *who) override
        {
            DoScriptText(SAY_AGGRO, me);
        }
    
        void MoveInLineOfSight(Unit *who) override
        {
            //Despawn Time Keeper
            if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == C_TIME_KEEPER)
            {
                if (me->IsWithinDistInMap(who,20.0f))
                {
                    DoScriptText(SAY_BANISH, me);
                    me->DealDamage(who, who->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                }
            }
    
            ScriptedAI::MoveInLineOfSight(who);
        }
    
        void KilledUnit(Unit *killer) override
        {
            DoScriptText(RAND(SAY_SLAY1,SAY_SLAY2), me);
        }
    
        void JustDied(Unit *victim) override
        {
            DoScriptText(SAY_DEATH, me);
    
            if (pInstance) {
                pInstance->SetData(DATA_DEJA, DONE);
                pInstance->SetData(TYPE_RIFT,SPECIAL);
            }
        }
    
        void UpdateAI(const uint32 diff) override
        {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //Arcane Blast
            if (ArcaneBlast_Timer < diff)
            {
                DoCast(me->GetVictim(), HEROIC(SPELL_ARCANE_BLAST, H_SPELL_ARCANE_BLAST));
                ArcaneBlast_Timer = 15000+rand()%10000;
            }else ArcaneBlast_Timer -= diff;
    
            //Arcane Discharge
            if (ArcaneDischarge_Timer < diff)
            {
                Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                DoCast(target,HEROIC(SPELL_ARCANE_DISCHARGE, H_SPELL_ARCANE_DISCHARGE));
                ArcaneDischarge_Timer = 20000+rand()%10000;
            }else ArcaneDischarge_Timer -= diff;
    
            //Time Lapse
            if (TimeLapse_Timer < diff)
            {
                DoScriptText(SAY_BANISH, me);
                DoCast(me, SPELL_TIME_LAPSE);
                TimeLapse_Timer = 15000+rand()%10000;
            }else TimeLapse_Timer -= diff;
            
            if (HeroicMode)
            {
                if (Attraction_Timer < diff)
                {
                    DoCast(me,SPELL_ATTRACTION);
                    Attraction_Timer = 25000+rand()%10000;
                }else Attraction_Timer -= diff;
            }
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_chrono_lord_dejaAI(creature);
    }
};


void AddSC_boss_chrono_lord_deja()
{
    new boss_chrono_lord_deja();
}
