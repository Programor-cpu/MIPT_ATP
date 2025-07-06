def check_if_cheaper_at_home(recipe, pantry, price_for_dish):
    final_cost = 0
    for ingredient, details in recipe.items():
        required = int(details["amount"].replace('ml', '').replace('g', ''))  
        price = details["price"]
        if ingredient in pantry:
            available = int(pantry[ingredient].replace('ml', '').replace('g', ''))
        else:
            available = 0
        dif = required-available
        if dif>0:
            final_cost += price*dif

    return price_for_dish>final_cost
