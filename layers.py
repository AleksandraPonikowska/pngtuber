from psd_tools import PSDImage
import os

os.makedirs('assets', exist_ok=True)
psd = PSDImage.open('file.psd')

# TODO: maybe use them as groups?
# TODO: also, change names to englihs...

look_for = ["r","l","biurko","p2","l2","usta2","usta1","oczy2","oczy1","cialo"]

for layer in psd:

    if layer.is_visible() and not layer.is_group():

        layer_name = (layer.name).lower().strip()

        if layer_name in look_for:
            rendered = layer.composite()
            rendered.save(f"assets/{layer.name}.png")
            look_for.remove(layer_name)

print(f"Couldn't find {look_for}")
