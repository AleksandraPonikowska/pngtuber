from psd_tools import PSDImage
import os

os.makedirs('assets', exist_ok=True)
psd = PSDImage.open('file.psd')

# TODO: maybe use them as groups?
# TODO: also, change names to englihs...

look_for = ["r","l","biurko","p2","l2","mouth_open","mouth_closed","oczy2","oczy1","body"]

for layer in psd:

    if layer.is_visible() and not layer.is_group():

        layer_name = (layer.name).lower().strip()

        if layer_name in look_for:
            rendered = layer.composite()
            rendered.save(f"build/assets/{layer.name}.png")
            look_for.remove(layer_name)

print(f"Couldn't find {look_for}")
