import math

###########################################################
# Compute the distance between a ray, defined by a point p and
# an angle alpha, and an horizontal segment [a,b]
# distance is math.inf if no intersection
#
# @param px, py coordinates of point p
# @param alpha angle of ray from x axis
# @param ax, ay coordinates of point a
# @param bx x coordinare of point b (here, by=ay)
#
# @return [distance, tx, ty] where t is the intersection point
#         None if no intersection
#
###########################################################
def Distance2HSegment(px, py, alpha, ax, ay, bx, by):

    #### DEBUG ####
    #print(f"Distance2HSegment {px},{py},{math.degrees(alpha)},{ax},{ay},{bx},{ay}")
    
    # Normalise alpha in ]-pi,pi]
    alpha = math.fmod(alpha, 2*math.pi)    
    # here, alpha is in ]-2pi, 2pi[
    if alpha < 0: 
        alpha = alpha + 2*math.pi
    # here, alpha is in [0, 2pi[
    if alpha > math.pi:
        alpha = alpha - 2*math.pi
    # finished, alpha is now in ]-pi, pi]
    
    # set default results
    distance = math.inf
    tx = None
    ty = None
    
    # if b is to the left of a, swap a and b so ax < bx
    if bx < ax:
        cx = ax
        ax = bx
        bx = cx
        
    if alpha == 0:
        # intersection only if py == ay
        if py == ay:
            # intesection only if py < by
            if px < bx:
                if px > ax:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, to the left of a
                    # intersection is a
                    distance = ay-py
                    tx = ax
                    ty = ay
            else:
                pass
        else:
            pass
    elif alpha == -math.pi:
        # intersection only if py == ay
        if py == ay:
            # intesection only if px > ax
            if px > ax:
                if px < bx:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, to the right of b
                    # intersection is b
                    distance = py - by
                    tx = bx
                    ty = ay
            else:
                pass
        else:
            pass
    elif (py > ay) and (alpha > 0):
        # p is above and facing up
        pass
    elif (py < ay) and (alpha < 0):
        # p is below and facing down
        pass
    elif (px < ax) and ((alpha > math.pi/2) or (alpha < -math.pi/2)):
        # p is to the left and facing left
        pass
    elif (px > bx) and ((alpha < math.pi/2) and (alpha > -math.pi/2)):
        # p is to the right and facing right
        pass
    else: # general case
        # o point on (a,b) with same x coordinate as p
        # The triangle (p,o,t) is square in o, 
        # so op=distance * cos(beta) and ot=distance * sin(beta)
        # with beta = pi/2-alpha 
        beta = math.pi/2-alpha
        distance = abs((ay-py)/math.cos(beta))
        tx = px + distance*math.sin(beta)
        if (ax<tx) and (tx<bx):
            ty = ay
        else:
            distance = math.inf
            tx = None

    #### DEBUG ####
    #print(f"  ==> {distance},{tx},{ty}")

    return [distance, tx, ty]
        
###########################################################
# Compute the distance between a ray, defined by a point p and
# an angle alpha, and a vertical segment [a,b]
# distance is math.inf if no intersection
#
# @param px, py coordinates of point p
# @param alpha angle of ray from x axis
# @param ax, ay coordinates of point a
# @param by y coordinare of point b (here, bx=ax)
#
# @return [distance, tx, ty] where t is the intersection point
#         None if no intersection
#
###########################################################
def Distance2VSegment(px, py, alpha, ax, ay, bx, by):

    #### DEBUG ####
    #print(f"Distance2VSegment {px},{py},{math.degrees(alpha)},{ax},{ay},{ax},{by}")
    
    # Normalise alpha in ]-pi,pi]
    alpha = math.fmod(alpha, 2*math.pi)    
    # here, alpha is in ]-2pi, 2pi[
    if alpha < 0: 
        alpha = alpha + 2*math.pi
    # here, alpha is in [0, 2pi[
    if alpha > math.pi:
        alpha = alpha - 2*math.pi
    # finished, alpha is now in ]-pi, pi]
    
    # set result to None
    distance = math.inf
    tx = None
    ty = None
    
    # if b is under a, swap a and b so ay < by
    if by < ay:
        cy = ay
        ay = by
        by = cy
        
    if alpha == math.pi/2:
        #### DEBUG
        #print("  Case 1 : alpha=90")
        
        # intersection only if px == ax
        if px == ax:
            # intesection only if py < by
            if py < by:
                if py > ay:
                    #### DEBUG
                    #print("p is on [a,b]")
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, under a
                    # intersection is a
                    #### DEBUG
                    #print("p is on the line, under a")
                    distance = ax - px
                    tx = ax
                    ty = ay
            else:
                pass
        else:
            pass
    elif alpha == -math.pi/2:
        #### DEBUG
        #print("  Case 2: alpha=-90")

        # intersection only if px == ax
        if px == ax:
            # intesection only if py > ay
            if py > ay:
                if py < by:
                    # p is on [a,b]
                    distance = 0
                    tx = px
                    ty = py
                else:
                    # p on the line, over b
                    # intersection is b
                    distance = px - bx
                    tx = ax
                    ty = by
            else:
                pass
        else:
            pass
    elif (py > by) and (alpha > 0):
        # p is above and facing up
        
        #### DEBUG
        #print("  Case 3 : p above segment")
        
        pass
    elif (py < ay) and (alpha < 0):
        # p is below and facing down

        #### DEBUG
        #print("  Case 4 : p below segment")
        
        pass
    elif (px < ax) and ((alpha > math.pi/2) or (alpha < -math.pi/2)):
        # p is to the left and facing left

        #### DEBUG
        #print("  Case 5 : p to the left")
        
        pass
    elif (px > ax) and ((alpha < math.pi/2) and (alpha > -math.pi/2)):
        # p is to the right and facing right

        #### DEBUG
        #print("  Case 6 : p to the right")
        
        pass
    else: # general case
        # o point on (a,b) with same y coordinate as p
        # The triangle (p,o,t) is square in o, 
        # so op=distance * cos(alpha) and ot=distance * sin(alpha)
        
        distance = abs((ax-px)/math.cos(alpha))
        ty = py + distance*math.sin(alpha)

        #### DEBUG
        #print(f"  Case 7 : good case, ty={ty}")        

        if (ay<ty) and (ty<by):
        
            #### DEBUG
            #print("    Case 7a : t on segment")
            tx = ax
        else:

            #### DEBUG
            #print("    Case 7b : t not on segment")

            distance = math.inf
            ty = None

    #### DEBUG ####
    #print(f"  ==> {distance},{tx},{ty}")

    return [distance, tx, ty]
