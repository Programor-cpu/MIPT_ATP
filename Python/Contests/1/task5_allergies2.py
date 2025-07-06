def find_top_allergies(k, **allergies):
    return [allergen_product for allergen_product, power in allergies.items() if k < power]


