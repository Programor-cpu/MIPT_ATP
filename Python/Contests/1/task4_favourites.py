def find_top_k_ingredients(all_foods, k, n):
    from collections import defaultdict
    ingredient_scores = defaultdict(lambda: {'rating': 0, 'count': 0})
    for food in all_foods:
        rating = food['rating']
        for ingredient in food['ingredients']:
            ingredient_scores[ingredient]['rating'] += rating
            ingredient_scores[ingredient]['count'] += 1

    
    ingredient_averege_scores = []

    for ingredient, scores in ingredient_scores.items():
        if n <= scores['count']:
            averege_score = scores['rating'] / scores['count']
            ingredient_averege_scores.append((ingredient, averege_score))

    
    ingredient_averege_scores.sort(key=lambda x: (-x[1], x[0]))
    top_k_ingredients = [ingredient[0] for ingredient in ingredient_averege_scores[:k]]
    top_k_ingredients.reverse()
    return top_k_ingredients
