
from scipy.stats import norm


def probability_in_range(mean, std, lower, upper):
    """
    Calculates the probability that a sample from a normal distribution
    falls between 'lower' and 'upper'.

    Parameters:
    mean (float): Mean of the normal distribution.
    std (float): Standard deviation of the normal distribution.
    lower (float): Lower bound of the interval.
    upper (float): Upper bound of the interval.

    Returns:
    float: Probability that the value falls between 'lower' and 'upper'.
    """
    p_lower = norm.cdf(lower, loc=mean, scale=std)
    p_upper = norm.cdf(upper, loc=mean, scale=std)
    return p_upper - p_lower


# Example usage:
mean = 7
std = 0.5
lower = 0
upper = 10

prob = probability_in_range(mean, std, lower, upper)
print(f"Probability between {lower} and {upper}: {prob:.4f}")
