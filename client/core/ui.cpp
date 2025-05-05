#include "ui.h"


/**
 *	TODO
 */
Button* UIBatch::add_button(const char* label,const char* tidle,const char* thover,const char* taction,
							vec2 position,vec2 scale)
{
	// graphics setup
	Button* out = buttons.next_free();
	out->idle = g_Renderer.register_sprite_texture(tidle);
	out->hover = g_Renderer.register_sprite_texture(thover);
	out->action = g_Renderer.register_sprite_texture(taction);
	out->canvas = g_Renderer.register_sprite(out->idle,position,scale);

	// intersection boundaries
	vec2 hscale = scale*vec2(.5f);
	out->bounds = {
		.position = position-hscale,
		.extent = position+hscale
	};
	return out;
}

/**
 *	TODO
 */
void UI::update()
{
	for (UIBatch* p_Batch : batches)
	{
		// button updates
		for (u16 i=0;i<p_Batch->buttons.active_range;i++)
		{
			Button& p_Button = p_Batch->buttons.mem[i];
			bool __Intersect = p_Button.bounds.intersect(g_Input.mouse.position);

			// button confirmation
			if (__Intersect&&g_Input.mouse.buttons[0])
			{
				g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.action);
				p_Button.holding = true;
				continue;
			}

			// button hover
			if (__Intersect)
			{
				g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.hover);
				p_Button.confirm = p_Button.holding;
			}

			// reset button state
			else g_Renderer.assign_sprite_texture(p_Button.canvas,p_Button.idle);
			p_Button.holding = false;
		}
	}
	// TODO check for removed ui batches during iteration
}
