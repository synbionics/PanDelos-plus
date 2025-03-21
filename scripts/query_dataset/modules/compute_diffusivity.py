def compute_diffusivity(families, number_of_genomes):
    fam_diffusivity = dict()
    diffusivity = dict()
    for i in range(number_of_genomes + 1):
        diffusivity[i] = {
            "number_of_families": 0,
            "families": list()
        }
    
    # print(type(families))
    for family_id, families in families.items():
        d = len(families["genomes"])
        fam_diffusivity[family_id] = d
        diffusivity[d]["number_of_families"] += 1
        diffusivity[d]["families"].append(family_id)

    if diffusivity[0]["number_of_families"] == 0:
        del diffusivity[0]
    
    return [fam_diffusivity, diffusivity]