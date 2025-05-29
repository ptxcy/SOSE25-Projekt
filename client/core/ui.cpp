#include "ui.h"


/**
 *	add a button to the batch
 *	\param label: button label writing across button surface
 *	\param tidle: idle texture, in case button is not interacted with this will be displayed
 *	\param thover: hover texture, in case mouse cursor hovers over button and button is not clicked
 *	\param taction: action texture, in case button is clicked and held
 *	\param position: center position of button
 *	\param scale: dimensions of the button
 *	\param alignment: (default neutral fullscreen) screen alignment within given borders
 *	\returns address of button to later read interaction state from
 */
lptr<Button> UIBatch::add_button(const char* label,string tidle,string thover,string taction,vec2 position,
								 vec2 scale,Alignment alignment)
{
	buttons.push_back({  });
	lptr<Button> p_Button = std::prev(buttons.end());

	// graphics setup
	p_Button->idle = g_Renderer.register_sprite_texture(tidle.c_str());
	p_Button->hover = g_Renderer.register_sprite_texture(thover.c_str());
	p_Button->action = g_Renderer.register_sprite_texture(taction.c_str());
	p_Button->canvas = g_Renderer.register_sprite(p_Button->idle,position,scale,.0f,1.f,alignment);

	// label text
	p_Button->label = g_Renderer.write_text(font,label,p_Button->canvas->offset,scale.y*.6f,vec4(1));

	// intersection boundaries
	vec2 hscale = scale*vec2(.5f);
	p_Button->bounds = {
		.position = p_Button->canvas->offset-hscale,
		.extent = p_Button->canvas->offset+hscale
	};
	return p_Button;
}

// TODO allow to only remove batch as a whole, as a consequence buttons dont have to be linked containers


/**
 *	update ui
 */
void UI::update()
{
	for (UIBatch& p_Batch : m_Batches)
	{
		// button updates
		for (Button& p_Button : p_Batch.buttons)
		{
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

/**
 *	create a ui batch
 *	\param font: button label and text box font for created batch
 */
lptr<UIBatch> UI::add_batch(Font* font)
{
	m_Batches.push_back({ .font = font });
	return std::prev(m_Batches.end());
}

// TODO routine to remove batch assets from renderer
