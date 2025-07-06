def find(ingredients, allergies):
    for i in allergies:
        if i in ingredients:
            return True
    return False

def find_allergies(all_foods, *allergies):
    allergy_list = [x["name"] for x in all_foods if find(x["ingredients"], allergies)==True ]
    return allergy_list
